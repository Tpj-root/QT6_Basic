

You can define your own macros that wrap qDebug(), qWarning(), qCritical() and add ANSI color codes automatically. 


Example:


```
#include <QDebug>

#define QINFO(msg)    qDebug() << "\033[32mINFO: " << msg << "\033[0m"      // Green
#define QWARN(msg)    qWarning() << "\033[33mWARNING: " << msg << "\033[0m"  // Yellow
#define QCRIT(msg)    qCritical() << "\033[31mCRITICAL: " << msg << "\033[0m"// Red

int main() {
    QINFO("This is an information message");
    QWARN("This is a warning message");
    QCRIT("This is a critical message");
}

```




---

### 1. Create a `.ts` file

* Run `lupdate` to extract translatable strings from your code:

```bash
lupdate day1.pro -ts i18n/day1_en.ts
```

* This creates a `.ts` file for English. For other languages, just change the suffix (`_fr.ts`, `_ta.ts`, etc.).

---

### 2. Add translatable strings in code

Use `QObject::tr()` for strings you want to translate:

```cpp
qDebug() << QObject::tr("Information message");
qWarning() << QObject::tr("Warning message");
qCritical() << QObject::tr("Critical message");
```

---

### 3. Edit `.ts` files

* Open them in **Qt Linguist** (`linguist day1_en.ts`)
* Provide translations for each string.
* Save changes.

---

### 4. Compile `.ts` to `.qm`

```bash
lrelease i18n/day1_en.ts
```

* This generates `day1_en.qm` in the same folder.

---

### 5. Load `.qm` in your app

You already have the loop for loading translators:

```cpp
QTranslator translator;
if (translator.load(":/i18n/day1_en.qm")) {
    a.installTranslator(&translator);
}
```

* `:/i18n/` → refers to **Qt Resource System**
* Or use relative path if you don’t use resources:

```cpp
translator.load("i18n/day1_en.qm");
```

---

### ✅ 6. Test

* Strings wrapped with `tr()` will now appear translated.
* Strings without `tr()` won’t be translated.

---



```cpp
qDebug() << QObject::tr("hellooo sab");
qDebug() << GREEN << QObject::tr("Information message") << RESET;
qWarning() << YELLOW << QObject::tr("Warning message") << RESET;
qCritical() << RED << QObject::tr("Critical message") << RESET;
```

Then run:

```bash
lupdate main.cpp -ts day1_ta_IN.ts
```

The `.ts` file will now contain entries you can translate in Qt Linguist.



`lupdate` now detected your strings! ✅

Next steps:

1. **Open the `.ts` file in Qt Linguist**:

```bash
linguist day1_ta_IN.ts
```

2. **Add Tamil translations** for each message. Example:

```xml
<message>
    <source>hellooo sab</source>
    <translation>ஹலோ சாப்</translation>
</message>
```

3. **Save the `.ts` file**.

4. **Compile to `.qm`**:

```bash
lrelease day1_ta_IN.ts
```

This creates `day1_ta_IN.qm`.

5. **Load in your app** (your current translator loop is fine).
   After that, all `tr()` strings will display in Tamil if the system locale matches `ta_IN`.

You can also keep using your **colored macros**; just wrap the messages in `tr()` like you did.





```

Open the .ts file in Qt Linguist:

linguist day1_ta_IN.ts



1. Compile .ts → .qm
lrelease day1_ta_IN.ts


This generates:

day1_ta_IN.qm





mkdir -p build/i18n
cp day1_ta_IN.qm build/i18n/






QTranslator translator;
const QStringList uiLanguages = QLocale::system().uiLanguages();
for (const QString &locale : uiLanguages) {
    QString baseName = "i18n/day1_" + QLocale(locale).name() + ".qm";
    if (translator.load(baseName)) {
        a.installTranslator(&translator);
        break;
    }
}

```


```cpp
#include <QCoreApplication>
#include <QTranslator>
#include <QDebug>

#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Load Tamil translation
    QTranslator translator;
    //translator.load("i18n/day1_ta_IN.qm");
    if (translator.load("day1_ta_IN.qm")) {  // make sure this file is in same folder as ./day1
        a.installTranslator(&translator);
    }

    // Colored messages with translations
    qDebug() << QObject::tr("hellooo sab");
    qDebug() << GREEN << QObject::tr("Information message") << RESET;
    qWarning() << YELLOW << QObject::tr("Warning message") << RESET;
    qCritical() << RED << QObject::tr("Critical message") << RESET;

    return 0; // no need for event loop in console app
}
```

---

### Steps to run:

1. Make sure `day1_ta_IN.qm` is in the same folder as `./day1` (build folder).
2. Build:

```bash
cd build
cmake ..
make
```

3. Run:

```bash
./day1
```

* All messages wrapped in `tr()` will now display in Tamil.
* Colors will still work in the terminal.






