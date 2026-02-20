// Vladislav Maur 
// Due Apr 14
// Pogram to assist equipment managers with keeping track of the teams equipment  

#include <iostream>
#include <occi.h>
#include <termios.h>
#include <unistd.h>
using namespace std;
using namespace oracle::occi;

// START OF QUERIES

const string VIEW_OFFERS_QUERY = "SELECT offer_id, gear_type, price FROM Offer";

const string VIEW_HISTORY_QUERY = "SELECT trans_id, offer_id, quantity, price, purchase_date FROM Transactions";

const string MAKE_OFFER_TRANSACTION_QUERY = "INSERT INTO Transactions (trans_id, offer_id, quantity, price, purchase_date) "
                                                "SELECT :1, :2, :3, price, SYSDATE FROM Offer WHERE offer_id = :2";
                                                
const string ADD_GEAR_QUERY = "UPDATE Gear SET num_available = num_available + :1 WHERE gear_id IN "
                                "(SELECT gear_id FROM Offer WHERE offer_id = :2)";

const string REMOVE_GEAR_QUERY = "UPDATE Gear SET num_available = num_available - 1 WHERE gear_id = :1";

const string MAKE_INCIDENT_CLAIM = "INSERT INTO Incident (inc_id, gear_id, i_description, date_reported) "
                                    "VALUES (:1, :2, :3, SYSDATE)";

const string VIEW_INVENTORY_QUERY = "SELECT gear_id, gear_type, gear_size, colour, num_available FROM Gear";

const string VIEW_INCIDENTS_QUERY = "SELECT inc_ID, gear_ID, i_description, date_reported FROM Incident";

// END OF QUERIES


string readPassword()
{
    struct termios settings;
    tcgetattr( STDIN_FILENO, &settings );
    settings.c_lflag =  (settings.c_lflag & ~(ECHO));
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );

    string password = "";
    getline(cin, password);

    settings.c_lflag = (settings.c_lflag |   ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
    return password;
}

//buy gear based on available offers
//deletes old offer, adds the transaction to history and adds gear to Gear table 
//asks user to enter offer ID and based on that adds it
void makeOffer(Connection *conn)
{
    string offerId, transId;
    int quantity;
    // Prompt user 
    cout << "Enter Transaction ID (make unique one): ";
    cin >> transId;
    cin.ignore(); 
    cout << "Enter Offer ID: ";
    cin >> offerId;
    cout << "Enter Quantity: ";
    cin >> quantity;

    try {

        // insert into Transactions table
        Statement *transStmt = conn -> createStatement(MAKE_OFFER_TRANSACTION_QUERY);
        transStmt -> setString(1, transId);
        transStmt -> setString(2, offerId);
        transStmt -> setInt(3, quantity);
        transStmt -> executeUpdate();
        
        // update Gear table
        Statement *gearStmt = conn -> createStatement(ADD_GEAR_QUERY);
        gearStmt -> setInt(1, quantity);
        gearStmt -> setString(2, offerId);
        gearStmt -> executeUpdate();
        
        // close statements
        conn -> terminateStatement(transStmt);
        conn -> terminateStatement(gearStmt);

        // Commit the transaction
        conn -> commit();

        cout << "Offer successfully made!" << endl;
    }
    catch (SQLException &e) {
        cout << e.what();
    } 
}

// look at different offers available from the offers table
// can quit to check back into main or call makeOffer() to buy the gear
void viewOffers(Connection *conn)
{
    char choice;
    do {
        try {
            Statement *stmt = conn -> createStatement();
            ResultSet *rs = stmt -> executeQuery(VIEW_OFFERS_QUERY);

            cout << "Available Offers:" << endl;
            cout << "-----------------" << endl;

            // Loop through the result set and print offer details
            while (rs->next()) 
            {
                cout << "Offer ID: " << rs -> getString(1) << endl;
                cout << "Gear Type: " << rs -> getString(2) << endl;
                cout << "Price: $" << rs -> getDouble(3) << endl;
                cout << "-----------------" << endl;
            }

            // Clean up
            stmt -> closeResultSet(rs);
            conn -> terminateStatement(stmt);
        }
        catch (SQLException &e) 
        {
            cout << e.what();
            return;
        }

        cout << "Choose an option:" << endl;
        cout << "1. Make Offer" << endl;
        cout << "2. Back to Main Menu" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); 

        switch (choice) 
        {
            case '1':
                makeOffer(conn);
                break;
            case '2':
                cout << "Returning to the main menu..." << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a valid option." << endl;
        }
    } while (choice != '2');
}

//function to view all transaction history 
void viewHistory(Connection *conn)
{
    try {
        Statement *stmt = conn -> createStatement();
        ResultSet *rs = stmt -> executeQuery(VIEW_HISTORY_QUERY);

        cout << "Transaction History:" << endl;
        cout << "---------------------" << endl;

        // Loop through the result set and print transaction details
        while (rs->next()) 
        {
            cout << "Transaction ID: " << rs -> getInt(1) << endl;
            cout << "Offer ID: " << rs -> getString(2) << endl;
            cout << "Quantity: " << rs -> getInt(3) << endl;
            cout << "Price: $" << rs -> getDouble(4) << endl;
            cout << "Purchase Date: " << rs -> getString(5) << endl;
            cout << "---------------------" << endl;
        }

        // Clean up
        stmt -> closeResultSet(rs);
        conn -> terminateStatement(stmt);
    }
    catch (SQLException &e) 
    {
        cout << e.what();
    }
}

// View available inventory 
void manageInventory(Connection *conn)
{
    try 
    {
        // Create statement and execute the query
        Statement *stmt = conn->createStatement();
        ResultSet *rs = stmt->executeQuery(VIEW_INVENTORY_QUERY);

        // Print header
        cout << "CURRENT INVENTORY:" << endl;
        cout << "-------------------------------------------" << endl;
        cout << "Gear ID\tGear Type\tSize\tColour\tQuantity" << endl;
        cout << "-------------------------------------------" << endl;

        // Loop through the result set and print gear details
        while (rs->next()) {
            cout << rs->getString(1) << "\t" // Gear ID
                 << rs->getString(2) << "\t" // Gear Type
                 << rs->getString(3) << "\t" // Size
                 << rs->getString(4) << "\t" // Colour
                 << rs->getInt(5) << endl;   // Quantity
            cout << "-------------------------------------------" << endl; 
        }

        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);
    }
    catch (SQLException &e) 
    {
        cout << e.what();
    }
}

// able to report an incident which will add it to the incidents and remove that gear from the gear table
void reportIncident(Connection *conn)
{
    string gearId, description, incId;

    // Prompt user for incident details
    cout << "Enter Incident ID (make a unique one): ";
    cin >> incId;
    cin.ignore(); 
    cout << "Enter Gear ID of the incident: ";
    cin >> gearId;
    cin.ignore(); 
    cout << "Enter description of the incident: ";
    getline(cin, description);

    try 
    {
        // Prepare the SQL statement to insert into Incident table
        Statement *stmt1 = conn->createStatement(MAKE_INCIDENT_CLAIM);
        stmt1 -> setString(1, incId);
        stmt1 -> setString(2, gearId);
        stmt1 -> setString(3, description);

        stmt1->executeUpdate();

        // update the availability of the gear in the Gear table
        Statement *stmt2 = conn->createStatement(REMOVE_GEAR_QUERY);
        stmt2 -> setString(1, gearId);
        stmt2->executeUpdate();

        conn->terminateStatement(stmt1);
        conn->terminateStatement(stmt2);

        conn->commit();

        cout << "Incident reported successfully!" << endl;
    } catch (SQLException &e) 
    {
        cout << e.what();
    }
}

void viewIncidents(Connection *conn)
{
    try {
        // Create statement and execute the query
        Statement *stmt = conn->createStatement();
        ResultSet *rs = stmt->executeQuery(VIEW_INCIDENTS_QUERY);

        // Print header
        cout << "INCIDENTS:" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << "Incident ID\tGear ID\tDescription\tDate Reported" << endl;
        cout << "-----------------------------------------------------" << endl;

        // Loop through the result set and print incident details
        while (rs->next()) {
            cout << rs->getString(1) << "\t\t" // Incident ID
                 << rs->getString(2) << "\t" // Gear ID
                 << rs->getString(3) << "\t" // Description
                 << rs->getString(4) << endl; // date
            cout << "-----------------------------------------------------" << endl; 
        }

        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);
    }
    catch (SQLException &e) {
        cout << e.what();
    }

}

int main()
{
    string userName;
    string password;
    // address of the Oracle server
    const string connectString = "database.csci.viu.ca";

    cout << "Your DB user name: ";
    getline(cin, userName);

    cout << "Your DB password: ";
    password = readPassword();
    cout << endl;
    try {
        // establish database connection
        Environment *env = Environment::createEnvironment();
        Connection *conn = env->createConnection
                          (userName, password, connectString);

        char choice;
        do {
            cout << "Choose an option:" << endl;
            cout << "1. Check and Make Offers" << endl;
            cout << "2. View Transaction History" << endl;
            cout << "3. View Inventory" << endl;
            cout << "4. Report Incident" << endl;
            cout << "5. View Incidents" << endl;
            cout << "6. Quit" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
            cin.ignore();

            switch (choice) 
            {
                case '1':
                    viewOffers(conn);
                    break;
                case '2':
                    viewHistory(conn);
                    break;
                case '3':
                    manageInventory(conn);
                    break;
                case '4':
                    reportIncident(conn);
                    break;
                case '5':
                    viewIncidents(conn);
                    break;
                case '6':
                    cout << "Exiting..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Please enter a valid option." << endl;
            }
        } while (choice != '6');

        //closing connections 
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    }
    catch (SQLException & e) 
    {
        cout << e.what();
        return 0;
    }
}