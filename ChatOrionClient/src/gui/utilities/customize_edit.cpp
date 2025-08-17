#include "customize_edit.h"

CustomizeEdit::CustomizeEdit(QWidget *parent):QLineEdit(parent), _max_len(20)
{
    this->setStyleSheet("border-radius: 5px;");
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitTextLength);
}

void CustomizeEdit::SetMaxLength(int maxLen)
{
    _max_len = maxLen;
}
