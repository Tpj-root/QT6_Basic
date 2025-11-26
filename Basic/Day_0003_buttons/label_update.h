#ifndef LABEL_UPDATE_H
#define LABEL_UPDATE_H

#include <QLabel>
#include <QString>

// 🎨 Our custom label class that inherits from QLabel
class ColorfulLabel : public QLabel
{
    Q_OBJECT  // This macro is needed for all Qt classes

public:
    // Constructor - like a birth certificate for our label
    ColorfulLabel(const QString& text, QWidget* parent = nullptr);
    
    // 🎨 Different style methods - like different outfits for our label
    void setHappyStyle();      // Bright and cheerful
    void setWarningStyle();    // For important messages
    void setInfoStyle();       // For information
    void setCustomStyle(const QString& bgColor, const QString& textColor, int fontSize = 14);
    
    // 🔧 Utility methods
    void makeRounded(int radius = 10);
    void addGlowEffect();
};

#endif // LABEL_UPDATE_H
