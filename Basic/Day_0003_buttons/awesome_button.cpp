#include "awesome_button.h"
#include <QIcon>

// Constructor
AwesomeButton::AwesomeButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    setMinimumSize(120, 40);
    setCursor(Qt::PointingHandCursor);  // 👆 Hand cursor when hovering
    
    // Connect signals to slots for interactive effects
    connect(this, &QPushButton::pressed, this, &AwesomeButton::onPress);
    connect(this, &QPushButton::released, this, &AwesomeButton::onRelease);
}

// 🎨 Primary Style - Main action button
void AwesomeButton::setPrimaryStyle()
{
    QString style = 
        "QPushButton {"
        "   background-color: #007bff;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0056b3;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #004085;"
        "}";
    
    setStyleSheet(style);
    m_normalStyle = style;
}

// ✅ Success Style - For positive actions
void AwesomeButton::setSuccessStyle()
{
    QString style = 
        "QPushButton {"
        "   background-color: #28a745;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #218838;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1e7e34;"
        "}";
    
    setStyleSheet(style);
    m_normalStyle = style;
}

// ❌ Danger Style - For delete/cancel actions
void AwesomeButton::setDangerStyle()
{
    QString style = 
        "QPushButton {"
        "   background-color: #dc3545;"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c82333;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #bd2130;"
        "}";
    
    setStyleSheet(style);
    m_normalStyle = style;
}

// ⚠️ Warning Style - For warnings
void AwesomeButton::setWarningStyle()
{
    QString style = 
        "QPushButton {"
        "   background-color: #ffc107;"
        "   color: black;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e0a800;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #d39e00;"
        "}";
    
    setStyleSheet(style);
    m_normalStyle = style;
}

// 🎨 Custom Style - Your own colors!
void AwesomeButton::setCustomStyle(const QString& bgColor, const QString& textColor, int fontSize)
{
    QString style = QString(
        "QPushButton {"
        "   background-color: %1;"
        "   color: %2;"
        "   font-size: %3px;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "   background-color: %1;"
        "   opacity: 0.8;"
        "}"
        "QPushButton:pressed {"
        "   background-color: %1;"
        "   opacity: 0.6;"
        "}"
    ).arg(bgColor).arg(textColor).arg(fontSize);
    
    setStyleSheet(style);
    m_normalStyle = style;
}

// 🔧 Make rounded corners
void AwesomeButton::makeRounded(int radius)
{
    QString currentStyle = styleSheet();
    setStyleSheet(currentStyle + QString("border-radius: %1px;").arg(radius));
}

// 📏 Size methods
void AwesomeButton::setSmallSize()
{
    setFixedSize(100, 35);
    setStyleSheet(styleSheet() + "font-size: 12px; padding: 5px 10px;");
}

void AwesomeButton::setMediumSize()
{
    setFixedSize(140, 45);
    setStyleSheet(styleSheet() + "font-size: 14px; padding: 10px 20px;");
}

void AwesomeButton::setLargeSize()
{
    setFixedSize(180, 55);
    setStyleSheet(styleSheet() + "font-size: 16px; padding: 15px 30px;");
}

// 🖼️ Add icon to button
void AwesomeButton::addIcon(const QString& iconPath)
{
    setIcon(QIcon(iconPath));
    setIconSize(QSize(20, 20));
}

// ✨ Interactive effects
void AwesomeButton::onPress()
{
    // Button gets smaller when pressed (visual feedback)
    setStyleSheet(styleSheet() + "padding: 8px 18px;");
}

void AwesomeButton::onRelease()
{
    // Button returns to normal size
    setStyleSheet(styleSheet() + "padding: 10px 20px;");
}
void AwesomeButton::onHover()
{
    // Add hover effect implementation
    setStyleSheet(styleSheet() + "background-color: #000000;");
}

void AwesomeButton::onLeave()
{
    // Add leave effect implementation  
    setStyleSheet(m_normalStyle);
}
