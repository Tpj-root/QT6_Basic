import os
from fastapi import FastAPI, Header, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from sqlalchemy import create_engine, Column, Integer, String, Numeric
from sqlalchemy.orm import declarative_base, sessionmaker

# Database config
DATABASE_URL = os.environ.get("DATABASE_URL", "mysql+pymysql://billing_user:StrongPassword123@localhost/billing_system")
API_KEY = os.environ.get("MY_API_KEY", "changeme")  # store in .bashrc or .env

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

def verify_api_key(api_key: str):
    if api_key != API_KEY:
        raise HTTPException(status_code=401, detail="Invalid API key")

@app.get("/products")
def get_products():
    db = SessionLocal()
    products = db.query(Product).all()
    db.close()
    return [{"id": p.id, "name": p.name, "price": float(p.price)} for p in products]

@app.post("/update-price")
def update_price(data: ProductUpdate, x_api_key: str = Header(...)):
    verify_api_key(x_api_key)
    db = SessionLocal()
    product = db.query(Product).filter(Product.id == data.id).first()
    if product:
        product.price = data.price
        db.commit()
        db.close()
        return {"status": "success"}
    db.close()
    return {"status": "fail", "message": "Product not found"}
