from flask import Flask, request, jsonify, render_template
import yfinance as yf

app = Flask(__name__)


@app.route("/")
def index():
    return render_template("index.html")


@app.route("/get_stock_data", methods=["POST"])
def get_stock_data():
    data = request.json
    stock_symbol = data["stock"]
    comparison_symbol = data["comparison"]
    start_date = data["date"]
    amount_invested = data["amount"]

    try:
        stock_data = yf.download(stock_symbol, start=start_date)
        comparison_data = yf.download(comparison_symbol, start=start_date)

        start_price = stock_data.iloc[0]["Close"]
        latest_price = stock_data.iloc[-1]["Close"]

        comparison_start_price = comparison_data.iloc[0]["Close"]
        comparison_latest_price = comparison_data.iloc[-1]["Close"]

        shares_bought = amount_invested / start_price
        comparison_shares_bought = amount_invested / comparison_start_price

        current_value = shares_bought * latest_price
        comparison_current_value = comparison_shares_bought * comparison_latest_price

        profit_loss = current_value - amount_invested
        comparison_profit_loss = comparison_current_value - amount_invested

        profit_loss_percentage = (profit_loss / amount_invested) * 100
        comparison_profit_loss_percentage = (
            comparison_profit_loss / amount_invested
        ) * 100

        stock_data_json = stock_data.to_json()
        comparison_data_json = comparison_data.to_json()

        return jsonify(
            {
                "start_price": start_price,
                "latest_price": latest_price,
                "shares_bought": shares_bought,
                "current_value": current_value,
                "profit_loss": profit_loss,
                "profit_loss_percentage": profit_loss_percentage,
                "comparison_start_price": comparison_start_price,
                "comparison_latest_price": comparison_latest_price,
                "comparison_shares_bought": comparison_shares_bought,
                "comparison_current_value": comparison_current_value,
                "comparison_profit_loss": comparison_profit_loss,
                "comparison_profit_loss_percentage": comparison_profit_loss_percentage,
                "stock_data": stock_data_json,
                "comparison_data": comparison_data_json,
            }
        )
    except Exception as e:
        return jsonify({"error": str(e)}), 500


if __name__ == "__main__":
    app.run(debug=True)
