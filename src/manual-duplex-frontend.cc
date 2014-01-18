/* manual-duplex-frontend - The frontend of the CUPS Manual Duplex Wrapper
   Copyright (C) 2014 Martin Kuettler

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

class DuplexDialog : public QDialog
{
    Q_OBJECT;

public:
    DuplexDialog(Config& conf);
    ~DuplexDialog();
};



int main(int argc, char** argv)
{
    Config c;
    int qArgc = 1;
    QApplication app(qArgc, argv);
    if (c.openDialog) {
        DuplexDialog d(c);
        d.show();
    }
    app.exec();
}
