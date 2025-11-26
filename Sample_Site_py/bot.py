import os
from telegram import Update
from telegram.ext import ApplicationBuilder, CommandHandler, ContextTypes
import requests

API_URL = "http://127.0.0.1:8000/update-price"
BOT_TOKEN = os.environ.get("TELEGRAM_BOT_TOKEN")  # from .bashrc
API_KEY = os.environ.get("MY_API_KEY")  # same key as FastAPI

async def update_price(update: Update, context: ContextTypes.DEFAULT_TYPE):
    try:
        pid = int(context.args[0])
        price = float(context.args[1])
        headers = {"X-API-KEY": API_KEY} if API_KEY else {}
        res = requests.post(API_URL, json={"id": pid, "price": price}, headers=headers)
        if res.json().get("status") == "success":
            await update.message.reply_text(f"Product {pid} updated to {price}")
        else:
            await update.message.reply_text(f"Failed to update: {res.json().get('message')}")
    except (IndexError, ValueError):
        await update.message.reply_text("Usage: /update <id> <price>")
    except Exception as e:
        await update.message.reply_text(f"Error: {e}")

if __name__ == "__main__":
    app = ApplicationBuilder().token(BOT_TOKEN).build()
    app.add_handler(CommandHandler("update", update_price))
    app.run_polling()

