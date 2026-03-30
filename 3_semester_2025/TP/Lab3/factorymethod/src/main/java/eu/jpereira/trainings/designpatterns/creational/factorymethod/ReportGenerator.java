
package eu.jpereira.trainings.designpatterns.creational.factorymethod;

public abstract class ReportGenerator {
	protected abstract Report createReport();

	public Report generateReport(ReportData data) {
		Report generatedReport = createReport();
		if (generatedReport != null) {
			generatedReport.generateReport(data);
		}
		return generatedReport;
	}

	public Report generateReport(ReportData data, String type) {
		DefaultReportGenerator def = new DefaultReportGenerator();
		return def.generateReport(data, type);
	}

}

