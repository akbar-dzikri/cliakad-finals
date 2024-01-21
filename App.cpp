#include <iostream>
#include <fstream>
#include <vector>
#include <json/json.h>

using namespace std;

const string filePath = "data.json";

void clearScreen()
{
#ifdef WINDOWS
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen()
{
#ifdef _WIN32
    system("pause");
#else
    system("read -n 1 -s -p 'Press any key to continue...'");
#endif
}

string toUpper(string &str)
{
    string result;
    for (char ch : str)
    {
        result += toupper(ch);
    }
    return result;
}

class jadwalClass
{
public:
    struct Jadwal
    {
        vector<string> matkul;
        vector<string> jam;
        vector<string> namaDosen;
        vector<string> ruangan;
    };

    vector<Jadwal> jadwalSemingu;

    jadwalClass(const Jadwal (&jadwalSeminguArray)[5]) : jadwalSemingu(jadwalSeminguArray, jadwalSeminguArray + 5) {}

    void tampilkanJadwal() const
    {
        string hari[5] = {"Senin", "Selasa", "Rabu", "Kamis", "Jum'at"};
        clearScreen();
        cout << "==== Jadwal Mata Kuliah ====" << endl;
        for (int i = 0; i < 5; i++)
        {

            cout << "Hari " << hari[i] << ":\n";
            cout << "----------------------------" << endl;
            for (int j = 0; j < jadwalSemingu[i].matkul.size(); j++)
            {
                cout << "Mata Kuliah: " << jadwalSemingu[i].matkul[j] << endl;
                cout << "Jam        : " << jadwalSemingu[i].jam[j] << endl;
                cout << "Nama Dosen : " << jadwalSemingu[i].namaDosen[j] << endl;
                cout << "Ruangan    : " << jadwalSemingu[i].ruangan[j] << endl;
                cout << "----------------------------" << endl;
            }
            cout << "\n\n";
        }
    }

    void pencarianJadwal() const
    {
        string inputHari;
        clearScreen();
        cout << "Masukkan hari yang ingin dicari: ";
        cin >> inputHari;

        bool isFound = false;
        string hari[5] = {"Senin", "Selasa", "Rabu", "Kamis", "Jum'at"};
        cout << "==== Jadwal Mata Kuliah ====" << endl;

        for (int i = 0; i < 5; i++)
        {
            if (toUpper(hari[i]) == toUpper(inputHari))
            {
                isFound = true;

                cout << "Hari " << hari[i] << ":\n";
                cout << "----------------------------" << endl;
                for (int j = 0; j < jadwalSemingu[i].matkul.size(); j++)
                {
                    cout << "Mata Kuliah: " << jadwalSemingu[i].matkul[j] << endl;
                    cout << "Jam        : " << jadwalSemingu[i].jam[j] << endl;
                    cout << "Nama Dosen : " << jadwalSemingu[i].namaDosen[j] << endl;
                    cout << "Ruangan    : " << jadwalSemingu[i].ruangan[j] << endl;
                    cout << "----------------------------" << endl;
                }
                cout << "\n\n";
                break;
            }
        }

        if (!isFound)
        {
            cout << "Pencarian tidak ditemukan.\n";
        }
    }
};

class User
{
public:
    string nim;
    string password;
    string nama;
    vector<string> absensi;
    Json::Value tugas;
    jadwalClass jadwal;

    User(const string &nim, const string &password, const jadwalClass &jadwal)
        : nim(nim), password(password), jadwal(jadwal) {}

    void lihatTugas()
    {
        char confirm;
        cout << "Tugas dari " << nama << ":\n";
        if (tugas.empty())
        {
            cout << "Belum ada tugas yang ditambahkan" << endl;
            cout << "Tambah tugas? [Y/T] : ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y')
            {
                tambahTugas();
            }
        }
        else
        {

            for (const string &tugasEntry : tugas.getMemberNames())
            {
                const string &namaTugas = tugasEntry;
                const Json::Value &detailTugas = tugas[namaTugas];
                cout << "  Nama Tugas : " << namaTugas << "\n";
                cout << "  Matkul : " << detailTugas["matkul"].asString() << "\n";
                cout << "  Deadline : " << detailTugas["deadline"].asString() << "\n";
                cout << "-------------------------\n\n";
            }

            cout << "Tambah tugas? [Y/T] : ";
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y')
            {
                tambahTugas();
            }
        }
    }

    void tambahTugas()
    {
        string namaTugas, matkul, deadline;

        cin.ignore();
        cout << "Masukkan nama tugas : ";
        getline(cin, namaTugas);

        cout << "Masukkan mata kuliah (matkul): ";
        getline(cin, matkul);

        cout << "Masukkan deadline (ex: 2024-01-16): ";
        getline(cin, deadline);

        tugas[namaTugas]["matkul"] = matkul;
        tugas[namaTugas]["deadline"] = deadline;

        Json::Value root;
        ifstream file(filePath);
        if (file.is_open())
        {
            file >> root;
            file.close();
        }

        int userIndex = getUserIndex(nim, root);
        root["users"][static_cast<int>(userIndex)]["tugas"] = tugas;

        ofstream outFile(filePath);
        if (outFile.is_open())
        {
            outFile << root;
            outFile.close();
            clearScreen();
            cout << "Berhasil Menambahkan tugas!\n";
            lihatTugas();
        }
        else
        {
            cout << "Gagal membuka file.\n";
        }
    }

    void absen()
    {
        char confirm;
        time_t currentTime = time(nullptr);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
        string currentAbsensi = buffer;

        cout << "Waktu saat ini adalah : " << currentAbsensi << "\n\nAbsen sekarang? [Y/T] : ";
        cin >> confirm;

        if (confirm == 'y' || confirm == 'Y')
        {
            absensi.push_back(currentAbsensi);
            cout << "Absen tercatat pada: " << currentAbsensi << "\n";

            Json::Value root;
            ifstream file(filePath);

            if (!file.is_open())
            {
                cout << "Gagal membuka file " << filePath << endl;
                return;
            }

            file >> root;
            file.close();

            int userIndex = getUserIndex(nim, root);

            if (userIndex != static_cast<int>(-1))
            {
                root["users"][static_cast<int>(userIndex)]["absensi"].append(currentAbsensi);

                ofstream outFile(filePath);

                if (outFile.is_open())
                {
                    outFile << root;
                    outFile.close();
                    cout << "Absensi berhasil disimpan!\n";
                }
                else
                {
                    cout << "Gagal membuka file.\n";
                }
            }
            else
            {
                cout << "NIM tidak ditemukan!\n";
            }
        }
    }

    void lihatJadwal() const
    {
        jadwal.tampilkanJadwal();
    }
    void cariJadwal() const
    {
        jadwal.pencarianJadwal();
    }

private:
    static int getUserIndex(const string &nim, const Json::Value &root)
    {
        const Json::Value &users = root["users"];
        for (int i = 0; i < users.size(); i++)
        {
            if (users[static_cast<int>(i)]["nim"].asString() == nim)
            {
                return i;
            }
        }
        return static_cast<int>(-1);
    }
};

class UserManager
{
public:
    static bool authenticateUser(const string &nim, const string &password, User &user, const jadwalClass &jadwal)

    {
        Json::Value root;
        ifstream file(filePath);

        if (!file.is_open())
        {
            cout << "Gagal membuka file " << filePath << endl;
            return false;
        }

        file >> root;
        file.close();

        int userIndex = getUserIndex(nim, root);
        if (userIndex != static_cast<int>(-1))
        {
            if (root["users"][static_cast<int>(userIndex)]["password"].asString() == password)
            {
                user = User(nim, password, jadwal);
                user.nama = root["users"][static_cast<int>(userIndex)]["nama"].asString();
                user.tugas = root["users"][static_cast<int>(userIndex)]["tugas"];
                cout << "Autentikasi Berhasil!" << endl;
                return true;
            }
            else
            {
                cout << "Password Salah!" << endl;
            }
        }
        else
        {
            cout << "NIM tidak ditemukan!" << endl;
        }

        return false;
    }

    static void registerUser(const string &nim, const string &password, const string &nama)
    {
        Json::Value root;
        ifstream file(filePath);

        if (file.is_open())
        {
            file >> root;
            file.close();
        }

        int existingUserIndex = getUserIndex(nim, root);
        if (existingUserIndex != static_cast<int>(-1))
        {
            cout << "NIM sudah terdaftar, silahkan login\n";
            cout << "NIM terdaftar sebagai :\n";
            cout << "NIM: " << root["users"][existingUserIndex]["nim"].asString() << endl;
            cout << "Nama: " << root["users"][existingUserIndex]["nama"].asString() << endl;
        }
        else
        {
            Json::Value newUser;
            newUser["nim"] = nim;
            newUser["nama"] = nama;
            newUser["password"] = password;
            newUser["tugas"] = Json::objectValue;

            root["users"].append(newUser);

            ofstream outFile(filePath);
            if (outFile.is_open())
            {
                outFile << root;
                outFile.close();
                cout << "Registrasi sukses!\n";
            }
            else
            {
                cout << "Gagal membuka file.\n";
            }
        }
    }

private:
    static int getUserIndex(const string &nim, const Json::Value &root)
    {
        const Json::Value &users = root["users"];
        for (int i = 0; i < users.size(); i++)
        {
            if (users[static_cast<int>(i)]["nim"].asString() == nim)
            {
                return i;
            }
        }
        return static_cast<int>(-1);
    }
};

int main()
{
    string nim, password, nama;
    char pilihan, ulangi, menu;
    jadwalClass::Jadwal senin{
        .matkul = {"Logika Informatika"},
        .jam = {"08.00-10.30"},
        .namaDosen = {"Kosim, S.Pd., M.Pd."},
        .ruangan = {"203"}};

    jadwalClass::Jadwal selasa{
        .matkul = {"IT English", "Fisika Dasar"},
        .jam = {"10.00-11.40", "13.00-15.30"},
        .namaDosen = {"Ijah Hadijah S.Pd., M.M.", "Suwandi, S.Pd., M.Pd."},
        .ruangan = {"102", "203"}};

    jadwalClass::Jadwal rabu{
        .matkul = {"Paket Aplikasi Komputer", "Agama"},
        .jam = {"10.30-13.00", "15.00-16:40"},
        .namaDosen = {"Dr. Yuhano, S.Kom., M.Kom.", "Matrudji, M.Ag"},
        .ruangan = {"Lab Multimedia", "102"}};

    jadwalClass::Jadwal kamis{
        .matkul = {"Pancasila", "Pengantar Teknologi Informasi"},
        .jam = {"08.00-09.40", "13.00-14.40"},
        .namaDosen = {"Igen Meisya, S.H", "Kosim, S.Pd., M.Pd."},
        .ruangan = {"205", "102"}};

    jadwalClass::Jadwal jumat{
        .matkul = {"Algoritma Pemrograman"},
        .jam = {"13.00-15.30"},
        .namaDosen = {"Faisal Akbar, S.Kom., M.T."},
        .ruangan = {"Lab Multimedia"}};

    jadwalClass::Jadwal jadwalSemingu[5] = {senin, selasa, rabu, kamis, jumat};

    jadwalClass jadwal(jadwalSemingu);

    do
    {
        clearScreen();
        cout << "====================================\n";
        cout << "      Selamat datang di cliakad\n";
        cout << "    Silahkan login terlebih dahulu\n";
        cout << "====================================\n";

        cout << "1. Login\n2. Register\n3. Keluar\n";
        cout << "Pilih : ";
        cin >> pilihan;

        switch (pilihan)
        {
        case '1':
            cout << "Masukkan NIM: ";
            cin >> nim;

            cout << "Masukkan password: ";
            cin >> password;
            break;
        case '2':
            cout << "Masukkan NIM: ";
            cin >> nim;

            cout << "Masukkan password: ";
            cin >> password;

            cin.ignore();
            cout << "Masukkan nama: ";
            getline(cin, nama);
            UserManager::registerUser(nim, password, nama);
            cout << "Kembali ke menu login? [y/t] : ";
            cin >> ulangi;
            continue;
        case '3':
            exit(0);
            break;
        default:
            while (pilihan != 1 && pilihan != 2)
            {
                cout << "Input tidak valid!\n";
                cout << "1. Login\n2. Register\n";
                cout << "Pilih : ";
                cin >> pilihan;
            }
            break;
        }
        User currentUser(nim, password, jadwal);
        if (UserManager::authenticateUser(nim, password, currentUser, jadwal))
        {
            while (true) // Main menu loop
            {
                clearScreen();
                cout << "Selamat datang " << currentUser.nama << "\nPilih menu dibawah :\n";
                cout << "1. Absen\n2. Menu Tugas\n3. Menu Jadwal\n4. Keluar\nPilih : ";
                cin >> menu;

                switch (menu)
                {
                case '1':
                    cin.ignore();
                    currentUser.absen();
                    break;
                case '2':
                    clearScreen();
                    currentUser.lihatTugas();
                    break;
                case '3':
                    cout << "1. Lihat Jadwal\n2. Cari Jadwal\n";
                    cin >> pilihan;
                    switch (pilihan)
                    {
                    case '1':
                        currentUser.lihatJadwal();
                        break;
                    case '2':
                        currentUser.cariJadwal();
                        break;
                    default:
                        cout << "Pilihan tidak Valid! Silahkan coba lagi.\n";
                    }
                    break;
                case '4':
                    exit(0);
                    break;
                default:
                    cout << "Pilihan tidak Valid! Silahkan coba lagi.\n";
                    break;
                }

                cout << "Kembali ke menu utama? [y/t] : ";
                cin >> ulangi;
                if (ulangi != 'y' && ulangi != 'Y')
                {
                    break;
                }
            }
        }
        else
        {
            cout << "Autentikasi gagal, silahkan coba lagi\n";
        }

        cout << "Kembali ke menu login? [y/t] : ";
        cin >> ulangi;
    } while (ulangi == 'y' || ulangi == 'Y');

    pauseScreen();
    return 0;
}