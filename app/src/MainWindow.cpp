#include "MainWindow.h"
#include "Block.h"
#include "DecoderFactory.h"
#include "SbfReader.h"
#include "Utils.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

// Global pointer used for redirecting qDebug() streams directly into the
static QTextEdit *g_logOutput = nullptr;

// Custom message handler callback to intercept Qt logging categories
void messageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg) 
{
  if (g_logOutput) {
    g_logOutput->append(msg);
  }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) 
{
  setupUI();
  setWindowTitle("RISE SBF Decoder - Graphical User Interface");
  resize(700, 500); // Default window dimensions

  // Connect the global interceptor pointer to our console widget
  g_logOutput = m_logConsole;
  qInstallMessageHandler(messageHandler);
}

void MainWindow::setupUI() 
{
  // Initialize the read-only file path display field
  m_filePathEdit = new QLineEdit(this);
  m_filePathEdit->setPlaceholderText("File path...");
  m_filePathEdit->setReadOnly(true);

  // Initialize layout trigger buttons
  m_browseFileButton = new QPushButton("Browse File", this);
  m_processButton = new QPushButton("Scan Binary", this);
  m_galRawFnavButton = new QPushButton("Scan Gal Raw FNAV (4022)", this);
  m_geoRawL1Button = new QPushButton("Scan GPS Raw L1 (4020)", this);
  m_gpsIonButton = new QPushButton("Scan GPS Ion (5893)", this);
  m_gpsRawCAButton = new QPushButton("Scan GPS Raw CA (4017)", this);
  m_measEpochButton = new QPushButton("Scan Meas Epoch (4027)", this);

  // Initialize the primary asynchronous telemetry logging area
  m_logConsole = new QTextEdit(this);
  m_logConsole->setReadOnly(true);

  // Assemble the top layout row (Path editing + Browse button)
  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->addWidget(m_filePathEdit);
  topLayout->addWidget(m_browseFileButton);

  // Assemble the secondary layout row for filtered execution buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(m_gpsRawCAButton);
  buttonLayout->addWidget(m_geoRawL1Button);
  buttonLayout->addWidget(m_galRawFnavButton);
  buttonLayout->addWidget(m_measEpochButton);
  buttonLayout->addWidget(m_gpsIonButton);

  // Organize everything inside the main vertical structural layout container
  QVBoxLayout *mainLayout = new QVBoxLayout();
  mainLayout->addLayout(topLayout);
  mainLayout->addWidget(m_processButton);
  mainLayout->addLayout(buttonLayout);

  mainLayout->addWidget(m_logConsole);
  // Encapsulate the main layout schema inside a safe central widget frame
  QWidget *centralWidget = new QWidget(this);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);

  connect(m_browseFileButton, &QPushButton::clicked, this, &MainWindow::onBrowseFileClicked);
  connect(m_processButton, &QPushButton::clicked, this, &MainWindow::onProcessFileClicked);
  connect(m_galRawFnavButton, &QPushButton::clicked, this, [this]() { onProcessFileClicked(Sbf::BlockId::GAL_RAW_FNAV); });
  connect(m_geoRawL1Button, &QPushButton::clicked, this, [this]() { onProcessFileClicked(Sbf::BlockId::GEO_RAW_L1); });
  connect(m_gpsIonButton, &QPushButton::clicked, this, [this]() { onProcessFileClicked(Sbf::BlockId::GPS_ION); });
  connect(m_gpsRawCAButton, &QPushButton::clicked, this, [this]() { onProcessFileClicked(Sbf::BlockId::GPS_RAW_CA); });
  connect(m_measEpochButton, &QPushButton::clicked, this, [this]() { onProcessFileClicked(Sbf::BlockId::MEAS_EPOCH); });
}

void MainWindow::onBrowseFileClicked() 
{
  // Launch standard OS native file dialog with target GNSS extension filters
  QString filePath = QFileDialog::getOpenFileName(this, "Select Binary File", 
                                                  "D:/RISE/sapt2/test4/app",
                                                "GNSS Files (*.26_);;All Files (*)");

  if (!filePath.isEmpty()) 
  {
    m_filePathEdit->setText(filePath);
    m_logConsole->append("[INFO] File selected: " + filePath);
  }
}

void MainWindow::onProcessFileClicked(quint16 filterId) 
{
  QString filePath = m_filePathEdit->text();
  if (filePath.isEmpty()) 
  {
    QMessageBox::warning(this, "Error", "You must select a file first!");
    return;
  }

  m_logConsole->append("[INFO] Processing file with SbfReader...");

  if (filterId == 0) 
  {
    m_logConsole->append("[INFO] Decoding ALL blocks...");
  } else 
  {
    m_logConsole->append("[INFO] Decoding only blocks with ID: " +
                         QString::number(filterId));
  }

  // Initialize backend RAII file stream layer mapping
  SbfReader reader(filePath);
  QMap<quint16, Block *> decoders; // Cache tracking one polymorphic object
  QMap<quint16, int> blockCounters; // Statistics monitoring structural occurrences count
  RawBlock rawBlock;
  int nTotalBlocks = 0;
  while (reader.readNextBlock(rawBlock)) 
  {
    nTotalBlocks++;
    if (nTotalBlocks >= 3000) 
    {
      m_logConsole->append("Limit of 3000 blocks reached, stopping processing.");
      break;
    }

    quint16 id = rawBlock.header.getRealId();

    // Active bitwise filtering bypass layer
    if (filterId != 0 && id != filterId) 
    {
      continue;
    }

    // Lazy-loading Creational Factory lookup pass
    if (!decoders.contains(id)) 
    {
      Block *newDecoder = DecoderFactory::createDecoder(id);
      if (newDecoder != nullptr) {
        decoders[id] = newDecoder;
      }
    }

    // Polymorphic block parsing execution and counter tracking
    Block *currentDecoder = decoders.value(id, nullptr);
    if (currentDecoder != nullptr) 
    {
      if (currentDecoder->decode(rawBlock)) 
      {
        blockCounters[id]++;
      }
    }

    // Periodic interface refresh step preventing window lockups (Not
    // Responding)
    if (nTotalBlocks % 100 == 0) 
    {
      m_logConsole->append("--> Processed " + QString::number(nTotalBlocks) +
                           " blocks...");
      QCoreApplication::processEvents();
    }
  }

  // Print consolidated summary metadata metrics on screen
  m_logConsole->append("=========================================");
  m_logConsole->append("Processing complete!");
  m_logConsole->append("Total blocks: " + QString::number(nTotalBlocks));
  m_logConsole->append("-----------------------------------------");
  for (auto it = blockCounters.begin(); it != blockCounters.end(); ++it) 
  {
    m_logConsole->append("  Block ID " + QString::number(it.key()) +
                         " -> decoded " + QString::number(it.value()) +
                         " times");
  }
  m_logConsole->append("=========================================");

  // Prevent memory leaks by safely clearing out alocated raw pointer blocks
  qDeleteAll(decoders);
  decoders.clear();
}

MainWindow::~MainWindow() 
{
  g_logOutput = nullptr;
  qInstallMessageHandler(nullptr);
}