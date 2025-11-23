import yfinance as yf
import pandas as pd
from datetime import datetime, timedelta

top_20_tickers = [
    'AAPL', 'MSFT', 'NVDA', 'GOOGL', 'AMZN',
    'META', 'BRK-B', 'TSLA', 'LLY', 'AVGO',
    'JPM', 'V', 'WMT', 'XOM', 'UNH', 'MA', 'PG', 'JNJ', 'HD', 'COST']

end_date = datetime.now()
start_date = end_date - timedelta(days=730)

print(f"Downloading stock data from {start_date.date()} to {end_date.date()}")
print("=" * 60)

print("\nDownloading combined data...")
try:
    combined_data = yf.download(top_20_tickers, start=start_date, end=end_date, group_by='ticker')
    combined_data.to_csv('all_stocks_combined.csv')
    print("✓ Saved: all_stocks_combined.csv")
except Exception as e:
    print(f"✗ Error downloading combined data: {e}")

print("\nDownloading individual stock files...")
for ticker in top_20_tickers:
    try:
        stock_data = yf.download(ticker, start=start_date, end=end_date, progress=False)
        if not stock_data.empty:
            stock_data.to_csv(f'{ticker}_data.csv')
            print(f"✓ Saved: {ticker}_data.csv ({len(stock_data)} rows)")
        else:
            print(f"✗ No data available for {ticker}")
    except Exception as e:
        print(f"✗ Error downloading {ticker}: {e}")

print("\n" + "=" * 60)
print("Download complete! CSV files created in current directory.")
print("\nCSV files include: Date, Open, High, Low, Close, Adj Close, Volume")
