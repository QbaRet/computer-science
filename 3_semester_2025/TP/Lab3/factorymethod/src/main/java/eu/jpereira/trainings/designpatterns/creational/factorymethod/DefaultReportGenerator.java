package eu.jpereira.trainings.designpatterns.creational.factorymethod;

/**
 * DefaultReportGenerator provides a convenience method to create reports by type.
 */
public class DefaultReportGenerator extends ReportGenerator {

    @Override
    protected Report createReport() {
        // Not used by DefaultReportGenerator.generateReport(reportData, type)
        return null;
    }

    public Report generateReport(ReportData data, String type) {
        ReportGenerator generator;
        if ("JSON".equalsIgnoreCase(type)) {
            generator = new JSONReportGenerator();
        } else if ("XML".equalsIgnoreCase(type)) {
            generator = new XMLReportGenerator();
        } else if ("HTML".equalsIgnoreCase(type)) {
            generator = new HTMLReportGenerator();
        } else if ("PDF".equalsIgnoreCase(type)) {
            generator = new PDFReportGenerator();
        } else {

            generator = new XMLReportGenerator();
        }
        return generator.generateReport(data);
    }

}
