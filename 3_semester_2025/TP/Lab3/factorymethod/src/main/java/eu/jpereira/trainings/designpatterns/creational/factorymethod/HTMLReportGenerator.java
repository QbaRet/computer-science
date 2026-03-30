package eu.jpereira.trainings.designpatterns.creational.factorymethod;

public class HTMLReportGenerator extends ReportGenerator {

    @Override
    protected Report createReport() {
        return new HTMLReport();
    }
}