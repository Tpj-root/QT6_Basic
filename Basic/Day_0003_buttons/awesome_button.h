#ifndef AWESOME_BUTTON_H
#define AWESOME_BUTTON_H

#include <QPushButton>
#include <QString>

// 🔘 Our custom awesome button class
class AwesomeButton : public QPushButton
{
    Q_OBJECT

public:
    // Constructor
    AwesomeButton(const QString& text, QWidget* parent = nullptr);
    
    // 🎨 Style methods
    void setPrimaryStyle();    // Main action button
    void setSuccessStyle();    // Success/OK button
    void setDangerStyle();     // Delete/Cancel button
    void setWarningStyle();    // Warning button
    void setCustomStyle(const QString& bgColor, const QString& textColor, int fontSize = 14);
    
    // ✨ Effects
    void addHoverEffect();     // Change color when mouse hovers
    void addPressEffect();     // Change when clicked
    void makeRounded(int radius = 15);
    void addIcon(const QString& iconPath);
    
    // 🔧 Sizes
    void setSmallSize();
    void setMediumSize();
    void setLargeSize();

private slots:
    void onHover();
    void onLeave();
    void onPress();
    void onRelease();

private:
    QString m_normalStyle;  // Remember original style
};

#endif // AWESOME_BUTTON_H
