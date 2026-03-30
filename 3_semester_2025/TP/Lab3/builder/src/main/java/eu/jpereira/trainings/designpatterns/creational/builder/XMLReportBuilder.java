package eu.jpereira.trainings.designpatterns.creational.builder;

import java.util.Iterator;

import eu.jpereira.trainings.designpatterns.creational.builder.model.SaleEntry;
import eu.jpereira.trainings.designpatterns.creational.builder.model.SoldItem;
import eu.jpereira.trainings.designpatterns.creational.builder.xml.XMLReportBody;

/**
 * Builder for XML report bodies.
 */
public class XMLReportBuilder {

    public XMLReportBody buildReport(SaleEntry saleEntry) {
        XMLReportBody reportBody = new XMLReportBody();

        reportBody.putContent("<sale><customer><name>");
        reportBody.putContent(saleEntry.getCustomer().getName());
        reportBody.putContent("</name><phone>");
        reportBody.putContent(saleEntry.getCustomer().getPhone());
        reportBody.putContent("</phone></customer>");

        reportBody.putContent("<items>");

        Iterator<SoldItem> it = saleEntry.getSoldItems().iterator();
        while (it.hasNext()) {
            SoldItem soldEntry = it.next();
            reportBody.putContent("<item><name>");
            reportBody.putContent(soldEntry.getName());
            reportBody.putContent("</name><quantity>");
            reportBody.putContent(String.valueOf(soldEntry.getQuantity()));
            reportBody.putContent("</quantity><price>");
            reportBody.putContent(String.valueOf(soldEntry.getUnitPrice()));
            reportBody.putContent("</price></item>");
        }

        reportBody.putContent("</items></sale>");

        return reportBody;
    }

}
