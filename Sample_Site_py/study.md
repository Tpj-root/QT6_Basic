
* **Backend:** Python + FastAPI
* **Database:** MySQL
* **Frontend:** Simple HTML + JavaScript
* **Telegram bot:** Python `python-telegram-bot` to update product prices

Here’s a working example.

---

### 1️⃣ MySQL Sample Data

```sql
CREATE DATABASE billing_system;
USE billing_system;

CREATE TABLE products (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50),
    price DECIMAL(10,2)
);

INSERT INTO products (name, price) VALUES
('Product 1', 10.50),
('Product 2', 20.00),
('Product 3', 15.75),
('Product 4', 8.99),
('Product 5', 12.30),
('Product 6', 18.00),
('Product 7', 25.50),
('Product 8', 5.75),
('Product 9', 30.00),
('Product 10', 7.80);
```

---

### 2️⃣ Backend (Python FastAPI)

Install dependencies:

```bash
pip install fastapi uvicorn pymysql sqlalchemy python-telegram-bot
```

`backend.py`:

```python
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from sqlalchemy import create_engine, Column, Integer, String, Numeric
from sqlalchemy.orm import declarative_base, sessionmaker

# Database config
DATABASE_URL = "mysql+pymysql://root:password@localhost/billing_system"
engine = create_engine(DATABASE_URL, echo=True)
SessionLocal = sessionmaker(bind=engine)
Base = declarative_base()

class Product(Base):
    __tablename__ = "products"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String(50))
    price = Column(Numeric(10,2))

# FastAPI
app = FastAPI()
app.add_middleware(CORSMiddleware, allow_origins=["*"], allow_methods=["*"], allow_headers=["*"])

class ProductUpdate(BaseModel):
    id: int
    price: float

@app.get("/products")
def get_products():
    db = SessionLocal()
    products = db.query(Product).all()
    db.close()
    return [{"id": p.id, "name": p.name, "price": float(p.price)} for p in products]

@app.post("/update-price")
def update_price(data: ProductUpdate):
    db = SessionLocal()
    product = db.query(Product).filter(Product.id == data.id).first()
    if product:
        product.price = data.price
        db.commit()
        db.close()
        return {"status": "success"}
    db.close()
    return {"status": "fail", "message": "Product not found"}
```

Run backend:

```bash
uvicorn backend:app --reload
```

---

### 3️⃣ Simple Web UI

`index.html`:

```html
<!DOCTYPE html>
<html>
<head>
    <title>Billing System</title>
</head>
<body>
<h2>Products</h2>
<table border="1" id="productTable">
    <tr>
        <th>ID</th>
        <th>Name</th>
        <th>Price</th>
    </tr>
</table>

<script>
async function loadProducts() {
    const res = await fetch("http://127.0.0.1:8000/products");
    const products = await res.json();
    const table = document.getElementById("productTable");

    products.forEach(p => {
        const row = table.insertRow();
        row.insertCell(0).innerText = p.id;
        row.insertCell(1).innerText = p.name;
        row.insertCell(2).innerHTML = `<input type="number" value="${p.price}" data-id="${p.id}" onchange="updatePrice(this)">`;
    });
}

async function updatePrice(input) {
    const id = input.dataset.id;
    const price = parseFloat(input.value);
    await fetch("http://127.0.0.1:8000/update-price", {
        method: "POST",
        headers: {"Content-Type": "application/json"},
        body: JSON.stringify({id: parseInt(id), price})
    });
}

loadProducts();
</script>
</body>
</html>
```

---

### 4️⃣ Telegram Bot for Price Updates

`bot.py`:

```python
from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, ContextTypes
import requests

API_URL = "http://127.0.0.1:8000/update-price"
BOT_TOKEN = "YOUR_BOT_TOKEN"

async def update_price(update: Update, context: ContextTypes.DEFAULT_TYPE):
    try:
        pid = int(context.args[0])
        price = float(context.args[1])
        res = requests.post(API_URL, json={"id": pid, "price": price})
        if res.json()["status"] == "success":
            await update.message.reply_text(f"Product {pid} updated to {price}")
        else:
            await update.message.reply_text("Failed to update")
    except:
        await update.message.reply_text("Usage: /update <id> <price>")

app = ApplicationBuilder().token(BOT_TOKEN).build()
app.add_handler(CommandHandler("update", update_price))

app.run_polling()
```

---

✅ Features:

1. Backend API with FastAPI and MySQL.
2. Web UI to view and edit prices.
3. Telegram bot to update prices remotely.

---








### FIX


```
sudo mysql -u root


```
