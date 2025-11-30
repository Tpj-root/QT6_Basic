// main.cpp
// Simple Qt polymorphism demo.
// Build with Qt6 (QApplication). Explanations are inline as comments.

#include <QApplication>    // QApplication for GUI apps
#include <QWidget>         // QWidget = base class for all visual widgets
#include <QPushButton>     // QPushButton = a button (child of QWidget)
#include <QLabel>          // QLabel = a text label (child of QWidget)
#include <QVBoxLayout>     // Layout to stack widgets vertically
#include <QDebug>          // qDebug for console messages
#include <typeinfo>        // for typeid (optional)


// A small helper function that accepts ANY widget via a QWidget* pointer.
// This demonstrates polymorphism: the function doesn't care which exact child it gets.
void addWidgetToLayout(QWidget *widget, QVBoxLayout *layout)
{
    // We only require the parameter to be a QWidget pointer.
    // That allows passing QPushButton*, QLabel*, etc. — all derived from QWidget.
    // The layout->addWidget call works with the base class pointer.
    layout->addWidget(widget);

    // For demonstration, print the actual runtime class name to console.
    // This shows that even though the function's parameter type is QWidget*,
    // the actual object has its own concrete type (QPushButton, QLabel).
    qDebug() << "Added widget of runtime type:" << widget->metaObject()->className();
}


int main(int argc, char *argv[])
{
    // QApplication must be created for any Qt GUI program.
    QApplication app(argc, argv);

    // Create a top-level widget (a window). This is a QWidget — the "mother".
    QWidget window;
    window.setWindowTitle("Polymorphism demo");

    // Create a vertical layout and set it on the window.
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Create specific child widgets:
    // These are concrete classes derived from QWidget.
    QPushButton *button = new QPushButton("Click me"); // child with button-specific APIs
    QLabel *label = new QLabel("Hello, label!");       // child that shows text

    // Demonstration 1:
    // Use the helper that expects QWidget* — polymorphism in action.
    addWidgetToLayout(button, layout); // pass QPushButton* as QWidget*
    addWidgetToLayout(label, layout);  // pass QLabel* as QWidget*

    // Demonstration 2:
    // Show that we can still call child-specific methods when we have a child pointer:
    button->setCheckable(true); // QPushButton only: make it toggleable
    label->setAlignment(Qt::AlignCenter); // QLabel only: center the text

    // Demonstration 3:
    // Suppose we only have a base-class pointer — QWidget* — but it's pointing to a button.
    QWidget *generic = button; // upcast: safe and very common
    generic->show();           // calls the correct widget's show() (virtual behavior)
    // Note: show() is implemented so the correct visual behavior happens for QPushButton.

    // If we need to call a child-only API while we have a base pointer, use qobject_cast:
    // qobject_cast is safe (returns nullptr if the cast fails).
    if (QPushButton *pb = qobject_cast<QPushButton*>(generic)) {
        // Now pb is a QPushButton* and we can call button-specific functions.
        pb->setText("Clicked me (via qobject_cast)"); // child-specific call
    }

    // Extra: connect the button's clicked signal to a lambda that demonstrates both
    // base-pointer and child-pointer behaviors at runtime.
    QObject::connect(button, &QPushButton::clicked, [&]() {
        qDebug() << "--- Button clicked ---";
        qDebug() << "As child pointer type:" << typeid(*button).name();
        qDebug() << "As base pointer type:" << typeid(*generic).name();
        // Call the parent implementation explicitly (forcing parent method) if needed:
        // Example: label->QWidget::setWindowTitle("..."); // not useful here, but possible.
    });

    // Finalize: show the window with layout containing both widgets.
    window.show();

    // Run the event loop.
    return app.exec();
}

