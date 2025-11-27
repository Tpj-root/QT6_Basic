// Include necessary Qt headers
#include <QApplication>      // Main application class that manages the GUI application
#include <QWidget>           // Base class for all UI objects (windows, buttons, etc.)
#include <QVBoxLayout>       // Vertical box layout to arrange widgets vertically
#include <QHBoxLayout>       // Horizontal box layout to arrange widgets horizontally
#include <QLineEdit>         // Single-line text input field
#include <QPushButton>       // Clickable button widget
#include <QLabel>            // Text display widget for showing results

// Create our main window class that inherits from QWidget
class SimpleCalculator : public QWidget 
{
    // Q_OBJECT macro is required for any class that uses signals/slots (Qt's communication system)
    Q_OBJECT

public:
    // Constructor - called when we create a new SimpleCalculator object
    SimpleCalculator(QWidget *parent = nullptr) : QWidget(parent) 
    {
        // Set the window title that appears in the title bar
        setWindowTitle("Simple Calculator");
        
        // Set the fixed window size (width, height) in pixels
        setFixedSize(400, 200);
        
        // Call our function to create and arrange all the UI elements
        setupUI();
    }

private slots:
    // This function runs when the Add button is clicked
    // 'slots' are special functions that respond to events like button clicks
    void addNumbers() 
    {
        // Get text from first input field and convert to double (decimal number)
        // toDouble() returns the number and a boolean indicating if conversion was successful
        bool ok1, ok2;
        double num1 = input1->text().toDouble(&ok1);  // Convert first input to number
        double num2 = input2->text().toDouble(&ok2);  // Convert second input to number
        
        // Check if both conversions were successful
        if (ok1 && ok2) {
            // Calculate the sum
            double result = num1 + num2;
            
            // Display the result in the label
            // QString::number() converts the number back to text for display
            resultLabel->setText("Result: " + QString::number(result));
            
            // Change label color to green for success
            resultLabel->setStyleSheet("color: green; font-weight: bold;");
        } else {
            // If conversion failed, show error message
            resultLabel->setText("Error: Please enter valid numbers!");
            
            // Change label color to red for error
            resultLabel->setStyleSheet("color: red; font-weight: bold;");
        }
    }

private:
    // Declare our UI elements as class members so we can access them in all functions
    QLineEdit *input1;       // First number input field
    QLineEdit *input2;       // Second number input field  
    QPushButton *addButton;  // Button to trigger addition
    QLabel *resultLabel;     // Label to display the result

    // Function to create and arrange all UI elements
    void setupUI() 
    {
        // Create a vertical layout to arrange widgets from top to bottom
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        
        // Create first input field for number 1
        input1 = new QLineEdit(this);
        input1->setPlaceholderText("Enter first number");  // Hint text when empty
        input1->setStyleSheet("padding: 5px; font-size: 14px;");  // CSS-like styling
        
        // Create second input field for number 2  
        input2 = new QLineEdit(this);
        input2->setPlaceholderText("Enter second number");
        input2->setStyleSheet("padding: 5px; font-size: 14px;");
        
        // Create the Add button
        addButton = new QPushButton("Add Numbers", this);
        addButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }"
                               "QPushButton:hover { background-color: #45a049; }");  // Color change on hover
        
        // Create the result display label
        resultLabel = new QLabel("Result will appear here", this);
        resultLabel->setAlignment(Qt::AlignCenter);  // Center the text
        resultLabel->setStyleSheet("font-size: 16px; padding: 10px; border: 1px solid #ccc; background-color: #f9f9f9;");
        
        // Add all widgets to the vertical layout in order
        mainLayout->addWidget(input1);      // First input at top
        mainLayout->addWidget(input2);      // Second input below first
        mainLayout->addWidget(addButton);   // Button below inputs
        mainLayout->addWidget(resultLabel); // Result label at bottom
        
        // Connect the button's "clicked" signal to our "addNumbers" slot function
        // When button is clicked, addNumbers() function will automatically be called
        connect(addButton, &QPushButton::clicked, this, &SimpleCalculator::addNumbers);
        
        // Set this layout as the main layout for our window
        setLayout(mainLayout);
    }
};

// Main function - starting point of every C++ program
int main(int argc, char *argv[]) 
{
    // Create the QApplication object that manages the GUI application
    // argc and argv are command line arguments passed to the program
    QApplication app(argc, argv);
    
    // Create our calculator window
    SimpleCalculator calculator;
    
    // Make the window visible on screen
    calculator.show();
    
    // Start the application's event loop - this keeps the program running
    // and waiting for user interactions (button clicks, etc.)
    return app.exec();
}

// Include the meta-object compiler output (needed when Q_OBJECT is used in .cpp file)
#include "main.moc"