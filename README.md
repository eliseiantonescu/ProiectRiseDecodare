# RISE SBF Decoder

Aplicatie desktop (Qt6 + C++) care citeste fisiere binare SBF (Septentrio Binary Format) si decodeaza manual, bit cu bit, blocurile de navigatie satelitara.

Practic, ia un fisier binar brut de la receptor si il parseaza: gaseste sync bytes (`$@`), valideaza CRC-ul, si extrage campurile din fiecare bloc.

## Blocuri suportate

- **GPSRawCA** (4017) – navigatie GPS C/A, 10 cuvinte de 32 biti
- **GEORawL1** (4020) – navigatie GEO/SBAS L1, 8 cuvinte
- **GALRawFNAV** (4022) – navigatie Galileo F/NAV, 8 cuvinte
- **MeasEpoch2** (4027) – masuratori: pseudorange, doppler, carrier phase, C/N0 (cu sub-blocuri tip 1 si tip 2)
- **GPSIon** (5893) – parametrii ionosferici Klobuchar (alpha0-3, beta0-3)

## Structura proiectului

```
app/
├── main.cpp              # entry point
├── include/
│   ├── Block.h           # interfata abstracta pt decodoare
│   ├── SbfBlock.h        # structurile header + raw block
│   ├── SbfReader.h       # citire binara secventiala
│   ├── DecoderFactory.h  # factory - creeaza decodorul pe baza ID-ului
│   ├── Utils.h           # constante, block IDs, functia CRC
│   ├── MainWindow.h      # fereastra principala
│   ├── GpsRawCA.h
│   ├── GEORawL1.h
│   ├── GalRawFnavDecoder.h
│   ├── MeasEpoch.h
│   └── GpsIon.h
└── src/                  # implementarile .cpp
```

## Cum functioneaza

1. `SbfReader` scaneaza fisierul byte cu byte cautand secventa `0x24 0x40`
2. Citeste header-ul (CRC, ID, length) si verifica CRC-16/CCITT (polinom `0x1021`)
3. `DecoderFactory` creeaza un decoder potrivit pe baza block ID-ului
4. Fiecare decoder extrage campurile cu `QDataStream` (little-endian) si valideaza valorile Do-Not-Use (TOW=0xFFFFFFFF, WNc=0xFFFF, etc.)
5. Rezultatele se afiseaza in consola din GUI

## Cerinte

- C++17
- Qt 6.11.1 (Core, Widgets, SerialPort, Network, Core5Compat)
- CMake >= 3.22


## Utilizare

Deschizi aplicatia, dai Browse si selectezi un fisier `.sbf` / `.26_`, apoi alegi ce bloc vrei sa decodezi (sau "Scan Binary" pt toate). Rezultatele apar in consola de log.
