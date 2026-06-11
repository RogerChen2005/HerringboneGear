#include "mainwindow.h"

#include <QApplication>
#include <QPalette>
#include <QEvent>
#include <QString>
#include <QFile>

// ── Theming ───────────────────────────────────────────────────────────────

void MainWindow::applyTheme()
{
    // setStyleSheet() itself posts a PaletteChange to this widget; guard so a
    // theme change can never recurse back into another setStyleSheet().
    if (applyingTheme_)
        return;
    applyingTheme_ = true;

    // Follow the OS: dark window background → dark control palette.
    const bool dark =
        QApplication::palette().color(QPalette::Window).lightness() < 128;

    // Tokens differ per scheme; the QSS layout is identical for both.
    struct Palette {
        const char* text;
        const char* card;        const char* cardBorder;
        const char* title;       const char* label;
        const char* input;       const char* inputBorder;
        const char* inputHover;  const char* inputFocusBg;
        const char* spinHover;
        const char* accent;      const char* accentHover; const char* accentPressed;
        const char* disabledBg;  const char* disabledText;
        const char* handle;
    };

    const Palette light{
        "#2b303b",
        "#ffffff", "#e3e6ec",
        "#3b82f6", "#4a5160",
        "#f7f8fa", "#d8dce4",
        "#b9c0cc", "#ffffff",
        "#e8ebf1",
        "#3b82f6", "#2f74e6", "#2664cc",
        "#e6e8ee", "#a6adba",
        "#e3e6ec"
    };
    const Palette darkP{
        "#e6e8ee",
        "#2a2d35", "#3a3f4b",
        "#5b9dff", "#aab2c0",
        "#33373f", "#454b57",
        "#5a6171", "#3a3f4b",
        "#3f4550",
        "#3b82f6", "#4a8cf7", "#2f74e6",
        "#33373f", "#6b7280",
        "#3a3f4b"
    };
    const Palette& p = dark ? darkP : light;

    // Theme-colored spinbox arrow images, served from the Qt resource bundle
    // (see resources/resources.qrc). The colors are baked into each PNG per scheme.
    const QString tag      = dark ? "dark" : "light";
    const QString upIcon   = ":/icons/arrow_up_"   + tag + ".png";
    const QString downIcon = ":/icons/arrow_down_" + tag + ".png";
    const QString upOff    = ":/icons/arrow_up_"   + tag + "_off.png";
    const QString downOff  = ":/icons/arrow_down_" + tag + "_off.png";

    // The QSS layout lives in resources/styles/app.qss as a %N-token template;
    // fill the tokens with the active scheme's colors and arrow images.
    QFile qss(":/styles/app.qss");
    if (!qss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        applyingTheme_ = false;
        return;
    }
    const QString tmpl = QString::fromUtf8(qss.readAll());

    setStyleSheet(QString(tmpl)
        .arg(p.text)         // %1
        .arg(p.card)         // %2
        .arg(p.cardBorder)   // %3
        .arg(p.title)        // %4
        .arg(p.label)        // %5
        .arg(p.input)        // %6
        .arg(p.inputBorder)  // %7
        .arg(p.inputHover)   // %8
        .arg(p.inputFocusBg) // %9
        .arg(p.spinHover)    // %10
        .arg(p.accent)       // %12 (no %11 used)
        .arg(p.accentHover)  // %13
        .arg(p.accentPressed)// %14
        .arg(p.disabledBg)   // %15
        .arg(p.disabledText) // %16
        .arg(p.handle)       // %17
        .arg(upIcon)         // %18
        .arg(downIcon)       // %19
        .arg(upOff)          // %20
        .arg(downOff));      // %21

    applyingTheme_ = false;
}

void MainWindow::changeEvent(QEvent* event)
{
    QMainWindow::changeEvent(event);
    // React only to OS-driven scheme changes. PaletteChange is intentionally
    // excluded: setStyleSheet() emits it, which would recurse.
    if (event->type() == QEvent::ApplicationPaletteChange ||
        event->type() == QEvent::ThemeChange)
        applyTheme();
}
