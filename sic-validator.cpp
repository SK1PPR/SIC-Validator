#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <iomanip>

using namespace std;

int length;
int byteCodeLength;
vector<bool> bits(32, false);

vector<char> valid_hex = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f'};

std::string int_to_hex(int i, int length)
{
    std::stringstream stream;
    stream << "0x"
           << std::setfill('0') << std::setw(length * 2)
           << std::hex << i;
    return stream.str();
}

void validate(string s)
{
    string hexString;
    if (s[1] == 'x')
    {
        hexString = s;
        length = s.length() / 2 - 1;
    }
    else
    {
        hexString = "0x" + s;
        length = s.length() / 2;
    }
    int value;
    istringstream ost(hexString);
    ost >> hex >> value;
    int store = value;
    byteCodeLength = length * 8;
    bits.resize(byteCodeLength);
    for (int i = 0; i < byteCodeLength; i++)
    {
        if (value & 1)
            bits[i] = true;
        else
            bits[i] = false;
        value = value >> 1;
    }
    reverse(bits.begin(), bits.end());

    if (length == 3)
        store = store & 0xFCFFFF;
    else if (length == 4)
        store = store & 0xFCFFFFFF;
    hexString = int_to_hex(store, length);
    string message = "";
    cout << endl;

    // Check if it is a valid 1 byte SIC/XE insctruction
    if (length == 1)
    {
        vector<string> valid_opcodes = {
            "C4", "C0", "F4", "C8", "F0", "F8"};
        string check = "";
        check += hexString[2];
        check += hexString[3];
        if (find(valid_opcodes.begin(), valid_opcodes.end(), check) == valid_opcodes.end())
        {
            message += "Opcode bytes do not match any valid opcode for 1 byte SIC/XE format";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }
        message += "Valid opcode with valid format exists\n";
        cout << " -----VALID----- \n";
        cout << message << endl;
        return;
    }

    // Check if it is a valid 2 byte SIC/XE instruction
    else if (length == 2)
    {
        vector<string> valid_opcodes = {
            "90", "B4", "A0", "9C", "98", "AC", "A4", "A8", "94", "B0", "B8"};
        string check = "";
        check += hexString[2];
        check += hexString[3];
        if (find(valid_opcodes.begin(), valid_opcodes.end(), check) == valid_opcodes.end())
        {
            message += "Opcode bytes do not match any valid opcode for 2 byte SIC/XE format";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }
        vector<char> valid_registers = {'0', '1', '2', '3', '4', '5', '6', '8', '9'};
        if (find(valid_registers.begin(), valid_registers.end(), hexString[4]) == valid_registers.end())
        {
            message += "Half-Byte represents invalid Register for 2 byte SIC/XE format";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }
        if (find(valid_registers.begin(), valid_registers.end(), hexString[5]) == valid_registers.end())
        {
            message += "Half-Byte represents invalid Register for 2 byte SIC/XE format";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }

        message += "Valid opcode with valid format exists\n";
        cout << " -----VALID----- \n";
        cout << message << endl;
        return;
    }

    // Check if it is a valid 3 byte SIC or SIC/XE instruction
    else if (length == 3)
    {
        // Check if it is a valid SIC instruction
        if (!bits[6] && !bits[7])
        {
            message += "Since bits 6 and 7 are RESET instruction must be of SIC format\n";
            string check = "";
            check += hexString[2];
            check += hexString[3];

            if (check == "4C")
            {
                if (hexString != "0x4C0000")
                {
                    message += "Opcode represents RSUB instruction which must be 4C0000\n";
                    cout << " -----INVALID----- \n";
                    cout << message << endl;
                    return;
                }
                else
                {
                    cout << " -----VALID----- \n";
                }
            }

            vector<string> valid_opcodes = {
                "18", "40", "28", "3C", "30", "34", "38", "48", "50", "08", "04", "20", "44", "D8", "0C", "54", "14", "E8", "10", "1C", "E0", "2C", "DC"};
            if (find(valid_opcodes.begin(), valid_opcodes.end(), check) == valid_opcodes.end())
            {
                message += "Opcode bytes do not match any valid opcode for SIC format \n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
            if (bits[8])
            {
                message += "Since bit 8 is SET instruction uses indexed mode of addressing\n";
            }
            cout << " -----VALID----- \n";
            cout << message << endl;
            return;
        }
        if (bits[11])
        {
            message += "Since \'e\' flag of opcode is SET it cannot be of format 3\n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }
        // It is an SIC/XE instruction
        else if (bits[6] && bits[7])
        {
            message += "We are using Simple mode of addressing\n";
            if (bits[9] && bits[10])
            {
                message += "B and P flag are both SET which is always invalid\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
            if (bits[8])
                message += "We are using Indexed mode of addressing\n";
        }
        else if (bits[6] && !bits[7])
        {
            message += "We are using Indirect mode of addressing\n";
            if (bits[8])
            {
                message += "X flag is SET, which is invalid for this mode of addressing\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
            if (bits[9] && bits[10])
            {
                message += "B and P flag are both SET which is always invalid\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
        }
        else
        {
            message += "We are using Immediate mode of addressing\n";
            if (bits[8])
            {
                message += "X flag is SET, which is invalid for this mode of addressing\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
            if (bits[9] && bits[10])
            {
                message += "B and P flag are both SET which is always invalid\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
        }

        string check = "";
        check += hexString[2];
        check += hexString[3];

        if (check == "4C")
        {
            if (hexString != "0x4C0000")
            {
                message += "Opcode represents RSUB instruction which must be 4C0000\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
            else
            {
                cout << " -----VALID----- \n";
            }
        }

        vector<string> valid_opcodes = {
            "18", "58", "40", "28", "88", "24", "64", "3C", "30", "34", "38", "48", "00", "68", "50", "70", "08", "6C", "74", "04", "D0", "20", "60", "44", "D8", "4C", "EC", "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84", "10", "1C", "5C", "E0", "2C", "DC"};
        if (find(valid_opcodes.begin(), valid_opcodes.end(), check) == valid_opcodes.end())
        {
            message += "Opcode bytes do not match any valid opcode for SIC/XE format \n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }

        message += "Valid opcode with valid format exists\n";
        cout << " -----VALID----- \n";
        cout << message << endl;
        return;
    }

    // Check if it is a valid 4 byte SIC/XE instruction
    else
    {
        message += "We are using extended mode of addressing\n";
        if (!bits[6] && !bits[7])
        {
            message += "Since bits 6 and 7 are RESET instruction must be of SIC format\n";
            message += "Since length of object code is not 3 bytes it cannot be of type SIC\n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }
        if (!bits[11])
        {
            message += "Since \'e\' flag of opcode is RESET it cannot be of format 4\n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }

        if (bits[9])
        {
            message += "B flag is SET which is invalid with this mode of addressing\n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }
        if (bits[10])
        {
            message += "P flag is SET which is invalid with this mode of addressing\n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }

        if (bits[6] && bits[7])
        {
            message += "We are using Simple mode of addressing\n";
            if (bits[8])
                message += "We are using Indexed mode of addressing\n";
        }
        if (bits[6] && !bits[7])
        {
            message += "We are using Indirect mode of addressing\n";
            if (bits[8])
            {
                message += "X flag is SET which is invalid with this mode of addressing (dose not allow indexed)\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
        }
        if (!bits[6] && bits[7])
        {
            message += "We are using Immediate mode of addressing\n";
            if (bits[8])
            {
                message += "X flag is SET which is invalid with this mode of addressing (dose not allow indexed)\n";
                cout << " -----INVALID----- \n";
                cout << message << endl;
                return;
            }
        }

        vector<string> valid_opcodes = {
            "18", "58", "40", "28", "88", "24", "64", "3C", "30", "34", "38", "48", "00", "68", "50", "70", "08", "6C", "74", "04", "D0", "20", "60", "44", "D8", "EC", "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84", "10", "1C", "5C", "E0", "2C", "DC"};
        string check = "";
        check += hexString[2];
        check += hexString[3];
        if (find(valid_opcodes.begin(), valid_opcodes.end(), check) == valid_opcodes.end())
        {
            message += "Opcode bytes do not match any valid opcode for SIC/XE format \n";
            cout << " -----INVALID----- \n";
            cout << message << endl;
            return;
        }

        message += "Valid opcode with valid format exists\n";
        cout << " -----VALID----- \n";
        cout << message << endl;
        return;
    }
}

void solve_single()
{
    cout << "Please enter the object code for validation\n If your object code is \'ffff\', possible formats are:\n 0xffff\n 0xFFFF\n ffff\n FFFF\n\n";
    string s;
    while (true)
    {
        cout << "Enter object code:";
        cin >> s;
        if (s[1] == 'x')
        {
            if (s[0] != '0')
            {
                cout << "Please enter an object code of valid format\n";
                continue;
            }
            if (s.length() != 4 && s.length() != 6 && s.length() != 8 && s.length() != 10)
            {
                cout << "Please enter an object code of valid format\n";
                continue;
            }
            length = s.length() / 2 - 1;
            bool br = true;
            for (int i = 2; i < length * 2 + 2; i++)
            {
                if (find(valid_hex.begin(), valid_hex.end(), s[i]) == valid_hex.end())
                {
                    cout << "Please enter an object code of valid format\n";
                    br = false;
                    break;
                }
            }
            for (int i = 2; i < length * 2 + 2; i++)
            {
                if (s[i] == 'f')
                    s[i] = 'F';
                if (s[i] == 'e')
                    s[i] = 'E';
                if (s[i] == 'd')
                    s[i] = 'D';
                if (s[i] == 'c')
                    s[i] = 'C';
                if (s[i] == 'b')
                    s[i] = 'B';
                if (s[i] == 'a')
                    s[i] = 'A';
            }

            if (br)
                break;
        }
        else
        {
            bool br = true;
            if (s.length() != 2 && s.length() != 4 && s.length() != 6 && s.length() != 8)
            {
                cout << "Please enter an object code of valid format\n";
                continue;
            }
            length = s.length() / 2;
            for (int i = 0; i < length * 2; i++)
            {
                if (find(valid_hex.begin(), valid_hex.end(), s[i]) == valid_hex.end())
                {
                    cout << "Please enter an object code of valid format\n";
                    br = false;
                    break;
                }
            }
            for (int i = 0; i < length * 2; i++)
            {
                if (s[i] == 'f')
                    s[i] = 'F';
                if (s[i] == 'e')
                    s[i] = 'E';
                if (s[i] == 'd')
                    s[i] = 'D';
                if (s[i] == 'c')
                    s[i] = 'C';
                if (s[i] == 'b')
                    s[i] = 'B';
                if (s[i] == 'a')
                    s[i] = 'A';
            }
            if (br)
                break;
        }
    }
    validate(s);
    return;
}

void batch_solve()
{
    int t;
    cout << "Enter number of opcodes: ";
    cin >> t;
    cout << "Enter opcodes in the following format 0x[opcode] or [opcode]\n";
    cout << "Where length of [opcode] must be an even integer between 2 and 8 (inclusive)\n";
    string opcodes[t];
    for (int i = 0; i < t; i++)
    {
        string s;
        cin >> s;
        if (s[1] == 'x')
        {
            if (s[0] != '0')
            {
                cout << "Please enter an object code of valid format\n";
                return;
            }
            if (s.length() != 4 && s.length() != 6 && s.length() != 8 && s.length() != 10)
            {
                cout << "Please enter an object code of valid format\n";
                return;
            }
            length = s.length() / 2 - 1;
            for (int i = 2; i < length * 2 + 2; i++)
            {
                if (find(valid_hex.begin(), valid_hex.end(), s[i]) == valid_hex.end())
                {
                    cout << "Please enter an object code of valid format\n";
                    return;
                }
            }
            for (int i = 2; i < length * 2 + 2; i++)
            {
                if (s[i] == 'f')
                    s[i] = 'F';
                if (s[i] == 'e')
                    s[i] = 'E';
                if (s[i] == 'd')
                    s[i] = 'D';
                if (s[i] == 'c')
                    s[i] = 'C';
                if (s[i] == 'b')
                    s[i] = 'B';
                if (s[i] == 'a')
                    s[i] = 'A';
            }
        }
        else
        {
            if (s.length() != 2 && s.length() != 4 && s.length() != 6 && s.length() != 8)
            {
                cout << "Please enter an object code of valid format\n";
                return;
            }
            length = s.length() / 2;
            for (int i = 0; i < length * 2; i++)
            {
                if (find(valid_hex.begin(), valid_hex.end(), s[i]) == valid_hex.end())
                {
                    cout << "Please enter an object code of valid format\n";
                    return;
                }
            }
            for (int i = 0; i < length * 2; i++)
            {
                if (s[i] == 'f')
                    s[i] = 'F';
                if (s[i] == 'e')
                    s[i] = 'E';
                if (s[i] == 'd')
                    s[i] = 'D';
                if (s[i] == 'c')
                    s[i] = 'C';
                if (s[i] == 'b')
                    s[i] = 'B';
                if (s[i] == 'a')
                    s[i] = 'A';
            }
        }
        opcodes[i] = s;
    }
    for (int i = 0; i < t; i++)
    {
        validate(opcodes[i]);
    }
}

int main()
{
    cout << "Welcome to SIC/XE instruction validator\n\n";
    int n;
    while (true)
    {
        cout << "(1) Check Validity of an Opcode\n";
        cout << "(2) Check Validity of batch of Opcodes\n";
        cout << "(0) Exit program\n";
        cin >> n;
        if (n == 0)
            break;
        else if (n == 1)
            solve_single();
        else if (n == 2)
            batch_solve();
        else
            cout << "Please input a valid option\n";
    }
    return 0;
}