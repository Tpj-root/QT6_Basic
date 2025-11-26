#include "label_update.h"
#include <QGraphicsDropShadowEffect>

// Constructor - this runs when we create a new ColorfulLabel
ColorfulLabel::ColorfulLabel(const QString& text, QWidget* parent)
    : QLabel(text, parent)  // Call the parent QLabel constructor
{
    // Set some default properties
    setAlignment(Qt::AlignCenter);
    setMinimumSize(150, 60);  // Minimum width: 150, height: 60
}

// 🎨 Happy Style - Bright and cheerful
void ColorfulLabel::setHappyStyle()
{
    setStyleSheet(
        "background-color: lightgreen;"
        "color: darkgreen;"
        "font-size: 16px;"
        "font-weight: bold;"
        "border: 2px solid green;"
        "border-radius: 10px;"
        "padding: 5px;"
    );
}

// ⚠️ Warning Style - For important messages
void ColorfulLabel::setWarningStyle()
{
    setStyleSheet(
        "background-color: yellow;"
        "color: red;"
        "font-size: 14px;"
        "font-weight: bold;"
        "border: 3px solid orange;"
        "border-radius: 8px;"
        "padding: 5px;"
    );
}

// 💡 Info Style - For information
void ColorfulLabel::setInfoStyle()
{
    setStyleSheet(
        "background-color: lightblue;"
        "color: darkblue;"
        "font-size: 14px;"
        "border: 2px solid blue;"
        "border-radius: 5px;"
        "padding: 5px;"
    );
}

// 🎨 Custom Style - You choose the colors!
void ColorfulLabel::setCustomStyle(const QString& bgColor, const QString& textColor, int fontSize)
{
    QString style = QString(
        "background-color: %1;"
        "color: %2;"
        "font-size: %3px;"
        "border: 2px solid %2;"
        "border-radius: 8px;"
        "padding: 5px;"
    ).arg(bgColor).arg(textColor).arg(fontSize);
    
    setStyleSheet(style);
}

// 🔧 Make rounded corners
void ColorfulLabel::makeRounded(int radius)
{
    QString currentStyle = styleSheet();
    setStyleSheet(currentStyle + QString("border-radius: %1px;").arg(radius));
}

// ✨ Add a glow effect (advanced feature!)
void ColorfulLabel::addGlowEffect()
{
    QGraphicsDropShadowEffect* glowEffect = new QGraphicsDropShadowEffect(this);
    glowEffect->setBlurRadius(15);
    glowEffect->setColor(Qt::blue);
    glowEffect->setOffset(0, 0);
    setGraphicsEffect(glowEffect);
}
