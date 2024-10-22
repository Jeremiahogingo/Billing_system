#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct items {
    char item[50];
    float price;
    int Qty;
};

struct orders {
    char customer[50];
    char date[20];
    int number_of_items;
    struct items itm[50];
};

// Functions to generate the Bills
void generate_bill_header(char name[50], char date[30]) {
    printf("\n\n");
    printf("\t Jerrylion Restaurant");
    printf("\n\t_______________________");
    printf("\nDate: %s", date);
    printf("\n\tInvoice to %s", name);
    printf("\n");
    printf("\n_________________________________\t\t\n");
    printf("Items \t\t");
    printf("Qty \t\t");
    printf("Total \t\t");
    printf("\n_________________________________\t\t\n");
    printf("\n");
}

void generate_bill_body(char item[50], int Qty, float price) {
    printf("%s \t\t", item);
    printf("%d \t\t", Qty);
    printf("%.2f \t\t", Qty * price);
    printf("\n");
}

void generate_bill_footer(float total) {
    printf("\n");
    float dis = 0.1 * total;
    float net_total = total - dis;
    float cgst = 0.09 * net_total, grand_total = net_total + 2 * cgst; 
    printf("\n_____________________________________\n");
    printf("Sub Total \t\t\t%.2f", total);
    printf("\nDiscount @10%% \t\t\t%.2f", dis); 
    printf("\n_____________________________________");
    printf("\nNet total\t\t\t%.2f", net_total);
    printf("\nCGST @9%% \t\t\t%.2f", cgst); 
    printf("\nSGST @9%% \t\t\t%.2f", cgst); 
    printf("\n______________________________________");
    printf("\nGrand Total\t\t\t%.2f", grand_total);
    printf("\n______________________________________");
}

int main() {
    float total;
    int No_of_items;
    int operation_option;
    struct orders ord;
    FILE *fp;
    char saveBill = 'y', contFlag = 'y';

    while (contFlag == 'y') {
        total = 0; 
        printf("\t\t====================LION SHOP===================\t\t\n");
        printf("\n\nPlease select your preferred operation: \n");
        printf("\n1. Generate Invoice");
        printf("\n2. Show all Invoices");
        printf("\n3. Search Invoice");
        printf("\n4. Exit");

        printf("\n\nYour choice: ");
        scanf("%d", &operation_option);
        fgetc(stdin);

        switch (operation_option) {
            case 1: // Generate and save a new invoice
                printf("Please enter the name of the customer: ");
                fgets(ord.customer, sizeof(ord.customer), stdin);
                ord.customer[strcspn(ord.customer, "\n")] = 0; // Remove newline
                strcpy(ord.date, __DATE__);
                printf("Please enter the total items: \t");
                scanf("%d", &No_of_items);
                
                // Validate the number of items
                if (No_of_items <= 0 || No_of_items > 50) {
                    printf("Invalid number of items. Please enter a value between 1 and 50.\n");
                    break;
                }

                ord.number_of_items = No_of_items; 

                for (int i = 0; i < No_of_items; i++) {
                    fgetc(stdin); // To consume newline
                    printf("\n\nPlease enter item %d: \t", i + 1);
                    fgets(ord.itm[i].item, sizeof(ord.itm[i].item), stdin); 
                    ord.itm[i].item[strcspn(ord.itm[i].item, "\n")] = 0; // Remove newline
                    printf("Please enter the quantity:\t");
                    scanf("%d", &ord.itm[i].Qty);
                    printf("Please enter the unit price:\t");
                    scanf("%f", &ord.itm[i].price);
                    
                    // Validate quantity and price
                    if (ord.itm[i].Qty < 0 || ord.itm[i].price < 0) {
                        printf("Quantity and price must be non-negative.\n");
                        i--; // Repeat this item input
                        continue;
                    }

                    total += ord.itm[i].Qty * ord.itm[i].price;
                }
                generate_bill_header(ord.customer, ord.date);
                for (int i = 0; i < ord.number_of_items; i++) {
                    generate_bill_body(ord.itm[i].item, ord.itm[i].Qty, ord.itm[i].price);
                }
                generate_bill_footer(total);

                // Ask user if they want to save the invoice
                printf("\nDo you want to save this invoice [y/n]:\t");
                scanf(" %c", &saveBill);

                if (saveBill == 'y') {
                    fp = fopen("RestaurantBill.dat", "ab");
                    if (fp == NULL) {
                        printf("\nError opening file to save the invoice.\n");
                        break;
                    }
                    size_t written = fwrite(&ord, sizeof(struct orders), 1, fp);
                    if (written == 1) {
                        printf("\nInvoice saved successfully!\n");
                    } else {
                        printf("\nError saving the invoice.\n");
                    }
                    fclose(fp);
                }
                break;

            case 2: // Show all saved invoices
                fp = fopen("RestaurantBill.dat", "rb");
                if (fp == NULL) {
                    printf("\nNo saved invoices found!\n");
                } else {
                    printf("\n  ***** Your Previous Invoices *****\n");
                    while (fread(&ord, sizeof(struct orders), 1, fp) == 1) {
                        // Check if fields are valid
                        if (strlen(ord.customer) > 0 && ord.number_of_items > 0 && ord.number_of_items <= 50) {
                            printf("Debug: Customer: %s, Date: %s, Number of Items: %d\n", ord.customer, ord.date, ord.number_of_items);
                            
                            float tot = 0;
                            generate_bill_header(ord.customer, ord.date);
                            for (int i = 0; i < ord.number_of_items; i++) {
                                generate_bill_body(ord.itm[i].item, ord.itm[i].Qty, ord.itm[i].price);
                                tot += ord.itm[i].Qty * ord.itm[i].price;
                            }
                            generate_bill_footer(tot);
                        } else {
                            printf("Invalid invoice data. Skipping...\n");
                        }
                    }
                    if (ferror(fp)) {
                        printf("Error reading file.\n");
                    }
                    fclose(fp);
                }
                break;

            case 3: // Search for a specific invoice
                printf("\nEnter the name of the customer: ");
                char customer_name[50];
                fgets(customer_name, sizeof(customer_name), stdin);
                customer_name[strcspn(customer_name, "\n")] = 0; // Remove newline
                fp = fopen("RestaurantBill.dat", "rb"); // Open for reading
                if (fp == NULL) {
                    printf("\nNo saved invoices found!\n");
                } else {
                    printf("\n  ***** Invoice of %s *****\n", customer_name);
                    int invoiceFound = 0;
                    while (fread(&ord, sizeof(struct orders), 1, fp) == 1) {
                        if (strcmp(ord.customer, customer_name) == 0) {
                            float tot = 0;
                            generate_bill_header(ord.customer, ord.date);
                            for (int i = 0; i < ord.number_of_items; i++) {
                                generate_bill_body(ord.itm[i].item, ord.itm[i].Qty, ord.itm[i].price);
                                tot += ord.itm[i].Qty * ord.itm[i].price;
                            }
                            generate_bill_footer(tot);
                            invoiceFound = 1;
                        }
                    }
                    if (!invoiceFound) {
                        printf("Sorry, no invoice found for %s.\n", customer_name);
                    }
                    if (ferror(fp)) {
                        printf("Error reading file.\n");
                    }
                    fclose(fp);
                }
                break;

            case 4:
                printf("\nExiting...\n");
                contFlag = 'n';
                break;

            default:
                printf("\nInvalid option! Please try again.\n");
        }

        if (operation_option != 4) {
            printf("\nDo you want to perform another operation? [y/n]: ");
            scanf(" %c", &contFlag);
        }
    }

    return 0;
}
