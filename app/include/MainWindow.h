#pragma once

#include <QHBoxLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>


class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  // Slot functions triggered on button click events
  void onBrowseFileClicked();
  void onProcessFileClicked(quint16 filterId);

private:
  // Initializes and constructs the user interface at startup
  void setupUI();

private:
  // UI Visual Components (Widgets)
  QLineEdit *m_filePathEdit;
  QPushButton *m_browseFileButton;
  QPushButton *m_processButton;
  QPushButton *m_geoRawL1Button;
  QPushButton *m_galRawFnavButton;
  QPushButton *m_gpsIonButton;
  QPushButton *m_gpsRawCAButton;
  QPushButton *m_measEpochButton;
  QTextEdit *m_logConsole;
};