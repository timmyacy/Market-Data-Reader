# CSV Market Data Reader

This is a command-line tool for computing daily return statistics and rolling volatility from OHLCV CSV files downloaded via `yfinance`.

## Features

- Interactive arrow-key file selector or direct ticker flag
- Mean daily return, annualised volatility, and max drawdown with trough date
- Rolling 20-day volatility exported as a second CSV
- Parses yfinance-format CSVs (multi-row headers handled automatically)

## Project Structure

```
CSV Market Data Reader/
├── include/
│   └── analyser.h        -----> structs and function declarations
├── src/
│   ├── analyser.cpp      -----> all function definitions
│   └── main.cpp          -----> entry point
├── CMakeLists.txt
└── README.md
```

## Requirements

- C++17 or later
- CMake 3.10+

## Build

```bash
cmake --build cmake-build-debug
```

## Usage

**Interactive mode** : It also used the arrow-key to select through any `.csv` files in the current directory:

```bash
./cmake-build-debug/main
```
The line above will run the execcutable

```
Select CSV file:

 > PLTR.csv
   AAPL.csv
```

**Symbol mode** : This will load a file directly by ticker, skipping the menu:

```bash
./cmake-build-debug/main --symbol AAPL
```

This expects `AAPL.csv` to exist in the current directory. Download it first with yfinance with the syntax below:

```python
import yfinance as yf
yf.download("AAPL", start="2023-01-01", end="2023-12-31").to_csv("AAPL.csv")
```

## Output

Printed to stdout:

```
The dates are from 2023-12-29 to 2023-01-03
Mean daily return : 0.1835%
Annualised vol    : 19.9543%
Max drawdown      : 14.9324%  (trough: 2023-10-26)
```

Rolling volatility written to `<SYMBOL>_rolling_vol.csv`:

```
Date,RollingVol
2023-02-01,0.142
2023-02-02,0.138
...
```

## Statistics Reference

| Stat | Formula |
|---|---|
| Daily return | `(close_t - close_{t-1}) / close_{t-1}` |
| Mean daily return | `sum(returns) / n` |
| Annualised vol | `stddev(returns) * sqrt(252)` |
| Max drawdown | `(peak - trough) / peak` on close prices |
| Rolling 20-day vol | Sample stddev of 20-return window * sqrt(252), computed daily |
