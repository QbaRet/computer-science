/**
 * Copyright 2011 Joao Miguel Pereira
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
package eu.jpereira.trainings.designpatterns.structural.facade;

import eu.jpereira.trainings.designpatterns.structural.facade.model.Book;
import eu.jpereira.trainings.designpatterns.structural.facade.model.Customer;
import eu.jpereira.trainings.designpatterns.structural.facade.model.DispatchReceipt;
import eu.jpereira.trainings.designpatterns.structural.facade.model.Order;
import eu.jpereira.trainings.designpatterns.structural.facade.service.BookDBService;
import eu.jpereira.trainings.designpatterns.structural.facade.service.CustomerDBService;
import eu.jpereira.trainings.designpatterns.structural.facade.service.CustomerNotificationService;
import eu.jpereira.trainings.designpatterns.structural.facade.service.OrderingService;
import eu.jpereira.trainings.designpatterns.structural.facade.service.WharehouseService;

/**
 * @author jpereira
 *
 */
public class BookstoreFacadeImpl implements BookstoreFacade {

	private CustomerDBService customerService;
	private BookDBService bookService;
	private OrderingService orderingService;
	private CustomerNotificationService customerNotificationService;
	private WharehouseService warehouseService;

	@Override
	public void placeOrder(String customerId, String isbn) {
		// Get the customer from the database
		Customer customer = customerService.findCustomerById(customerId);

		// Get the book from the database
		Book book = bookService.findBookByISBN(isbn);

		// Create an order
		Order order = orderingService.createOrder(customer, book);

		// Dispatch the order
		DispatchReceipt receipt = warehouseService.dispatch(order);

		// Notify the customer
		customerNotificationService.notifyClient(receipt);
	}

	public void setCustomerService(CustomerDBService customerService) {
		this.customerService = customerService;
	}

	public void setBookService(BookDBService bookService) {
		this.bookService = bookService;
	}

	public void setOrderingService(OrderingService orderingService) {
		this.orderingService = orderingService;
	}

	public void setCustomerNotificationService(CustomerNotificationService customerNotificationService) {
		this.customerNotificationService = customerNotificationService;
	}

	public void setWharehouseService(WharehouseService warehouseService) {
		this.warehouseService = warehouseService;
	}
}
