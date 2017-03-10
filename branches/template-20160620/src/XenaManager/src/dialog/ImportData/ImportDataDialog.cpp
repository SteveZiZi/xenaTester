#include "ImportDataDialog.h"

#include "iedShowWidget.h"
#include "pcapShowDialog.h"

ImportDataDialog* CreateImportDataDlg(IMPORT_FILE_STYLE fileType, QWidget* parent)
{
    if (fileType == IMPORT_SCD_FILE) {
        return new CIedShowWidget(parent);
    }

    return new CPcapShowDialog(parent);
}

void ReleaseImportDataDlg(ImportDataDialog* dlg)
{
    if (dlg) {
        dlg->deleteLater();
    }
}