const historyList = [];

function renderHistoryButtons() {
  const historyContainer = document.getElementById("history");
  historyContainer.innerHTML = "";
  const header = document.createElement("h3");
  header.innerText = "Recent History";
  historyContainer.appendChild(header);

  historyList.forEach((history, index) => {
    const button = document.createElement("button");
    button.innerText = `${history.stock} vs ${history.comparison} - ${history.date} - $${history.amount}`;
    button.addEventListener("click", function () {
      const { stock, date, amount, comparison } = history;
      update(stock, date, amount, comparison);
    });

    historyContainer.appendChild(button);
  });
}

document
  .getElementById("stock-form")
  .addEventListener("submit", function (event) {
    event.preventDefault();
    const stock = document.getElementById("stock").value.toUpperCase();
    const date = document.getElementById("date").value;
    const amount = document.getElementById("amount").value;
    const comparison = document.getElementById("comparison").value;
    // Create a history object to store the stock data, amount, and comparison
    const history = {
      stock: stock,
      date: date,
      amount: amount,
      comparison: comparison,
    };

    // Add the history object to a list
    historyList.push(history);

    // Ensure the history list has a maximum length of 5
    if (historyList.length > 8) {
      historyList.shift();
    }

    update(stock, date, amount, comparison);
  });

function update(stock, date, amount, comparison) {
  renderHistoryButtons();
  fetch("/get_stock_data", {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({
      stock: stock,
      date: date,
      amount: parseFloat(amount),
      comparison: comparison,
    }),
  })
    .then((response) => response.json())
    .then((data) => {
      if (data.error) {
        document.getElementById(
          "result"
        ).innerHTML = `<p>Error: ${data.error}</p>`;
      } else {
        const profitLoss = parseFloat(data.profit_loss.toFixed(2));
        const profitLossColor = profitLoss > 0 ? "green" : "red";
        const comparisonProfitLoss = parseFloat(
          data.comparison_profit_loss.toFixed(2)
        );
        const comparisonProfitLossColor =
          comparisonProfitLoss > 0 ? "green" : "red";
        const profitLossHeader =
          profitLoss > 0
            ? `You made a profit of $${Math.abs(profitLoss).toFixed(2)}!`
            : `You made a loss of $${Math.abs(profitLoss).toFixed(2)} :(`;
        console.log(
          profitLoss > comparisonProfitLoss,
          profitLoss,
          comparisonProfitLoss,
          typeof profitLoss,
          typeof comparisonProfitLoss
        );
        const profitLossComparison =
          profitLoss > comparisonProfitLoss
            ? `You beat ${comparison} by $${Math.abs(
                profitLoss - comparisonProfitLoss
              ).toFixed(2)}!`
            : `However, you could have made an extra $${Math.abs(
                comparisonProfitLoss - profitLoss
              ).toFixed(2)} if you invested in ${comparison}!`;
        document.getElementById("result").innerHTML = `
        <h2 style="color: ${profitLossColor}">${profitLossHeader}</h2>
        <p style="color: ${profitLossColor}">${profitLossComparison}</p>
          <table>
            <tr>
              <th>Values</th>
              <th>${stock}</th>
              <th>${comparison}</th>
            </tr>
            <tr>
              <td>Bought ($)</td>
              <td>${data.start_price.toFixed(2)}</td>
              <td>${data.comparison_start_price.toFixed(2)}</td>
            </tr>
            <tr>
              <td>Current ($)</td>
              <td>${data.latest_price.toFixed(2)}</td>
              <td>${data.comparison_latest_price.toFixed(2)}</td>
            </tr>
            <tr>
            <td>Shares</td>
              <td>${data.shares_bought.toFixed(4)}</td>
              <td>${data.comparison_shares_bought.toFixed(4)}</td>
            </tr>
            <tr>
            <td>Current Value ($)</td>
              <td>${data.current_value.toFixed(2)}</td>
              <td>${data.comparison_current_value.toFixed(2)}</td>
            </tr>
            <tr>
              <td>Profit/Loss (%)</td>
              <td><span style="color: ${profitLossColor}">${data.profit_loss_percentage.toFixed(
          2
        )}%</span></td>
              <td><span style="color: ${comparisonProfitLossColor}">${data.comparison_profit_loss_percentage.toFixed(
          2
        )}%</span></td>
            </tr>
            <tr>
              <td>Profit/Loss ($)</td>
              <td><span style="color: ${profitLossColor}">$${profitLoss}</span></td>
              <td><span style="color: ${comparisonProfitLossColor}">$${comparisonProfitLoss}</span></td>
            </tr>
          </table>
          <div id="buttonContainer"></div>
        `;

        const now = new Date();
        const originalDate = new Date(date);
        const timeDifference = now.getTime() - originalDate.getTime();
        const daysDifference = Math.floor(timeDifference / (1000 * 3600 * 24));

        const tweetButton = document.createElement("button");
        tweetButton.innerText = profitLoss > 0 ? "Share Profit" : "Share Loss";
        tweetButton.addEventListener("click", function () {
          const tweetText = `I have achieved a ${
            profitLoss > 0 ? "profit" : "loss"
          } of $${Math.abs(profitLoss).toFixed(
            2
          )} in my ${stock} stock investment over ${daysDifference} days! Discover your investment results on UpOrDown.com #StockMarket #Investing #UpOrDown #${stock}`;
          const tweetUrl = `https://twitter.com/intent/tweet?text=${encodeURIComponent(
            tweetText
          )}`;
          window.open(tweetUrl);
        });
        const buttonContainer = document.getElementById("buttonContainer");
        buttonContainer.appendChild(tweetButton);

        if (data.profit_loss > 0) {
          confetti.start();
          setTimeout(function () {
            confetti.stop();
          }, 1000);
        }

        const stockData = JSON.parse(data.stock_data);
        const comparisonData = JSON.parse(data.comparison_data);
        const stockDates = Object.keys(stockData["Close"]);
        const stockClosingPrices = Object.values(stockData["Close"]);
        const comparisonDates = Object.keys(comparisonData["Close"]);
        const comparisonClosingPrices = Object.values(comparisonData["Close"]);

        const formattedStockDates = stockDates.map((timestamp) => {
          const date = new Date(parseInt(timestamp));
          return date.toISOString().split("T")[0];
        });

        const formattedComparisonDates = comparisonDates.map((timestamp) => {
          const date = new Date(parseInt(timestamp));
          return date.toISOString().split("T")[0];
        });

        const stockTrace = {
          x: formattedStockDates,
          y: stockClosingPrices,
          type: "scatter",
          mode: "lines",
          name: `${stock}`,
          line: { color: "#1f77b4" },
        };

        const comparisonTrace = {
          x: formattedComparisonDates,
          y: comparisonClosingPrices,
          type: "scatter",
          mode: "lines",
          name: `${comparison}`,
          line: { color: "#ff7f0e" },
        };

        const layout = {
          title: `${stock} vs ${comparison}`,
          xaxis: {
            title: "Date",
            tickfont: { color: "#e0e0e0" },
            titlefont: { color: "#e0e0e0" },
            gridcolor: "#333333",
          },
          yaxis: {
            title: "Share Price (USD)",
            tickfont: { color: "#e0e0e0" },
            titlefont: { color: "#e0e0e0" },
            gridcolor: "#333333",
          },
          plot_bgcolor: "#1e1e1e",
          paper_bgcolor: "#1e1e1e",
          font: { color: "#e0e0e0" },
        };

        Plotly.newPlot("graph", [stockTrace, comparisonTrace], layout);
      }
    })
    .catch((error) => {
      document.getElementById(
        "result"
      ).innerHTML = `<p>Error: ${error.message}</p>`;
    });
}
