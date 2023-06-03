#include <wx/wxprec.h>
#include <wx/toplevel.h>
#include <wx/window.h>
#include <wx/dc.h>
#include <wx/sizer.h>
#include <wx/wx.h>
#include <wx/timer.h>
//librarii standard
#include <time.h>
#include <chrono>
#include <stdlib.h>
#include <fstream>
#include <sstream>

/*	 ________________________
	/       *Hello*         /
   /       Made by:        /
  /      -AkabJack-       /
 /    Check my Twitter   /
/_______________________/
*/

static bool running = false; // bool pentru status
bool status_graf_one = false, status_graf_sec = false;
char status_mini_ecran = 'A'; // char pentru ferificare, poate avea doar 4 valori, default (A) duce la instructiuni;
wxSize marime_buton(120, 20);//(lungime , latime)

wxPoint pozitie_exit_button(260, 180);// pozitia butonului Exit MyFrame
wxPoint pozitie_loading_button(260, 60);// pozitia butonului 2 MyFrame
wxPoint pozitie_bubble_sort(260, 100);
wxPoint pozitie_insertion_sort(260, 141);

wxPoint pozitie_min_max(20, 60);
wxPoint afisare_numere_sortate(20, 100);
wxPoint pozitie_buton_rand(20, 141);
wxPoint pozitie_instructiuni(20, 180);

//constante si variabile normale
std::ifstream fisier;//stream pentru fisier
std::stringstream streamString, miniString;//stream pentru convertorul de string
int lista_Neordonate[101];
int lista_Ordonate[101];
int lista_request[3];
std::string conversie_string(int lista[], int index);//pentru conversie din array in string, pentru afisare
void operatiFisier(int i, std::string filename, int lista[]);//functie pentru indexare dintr-un fisier
void bubble_sort(int arr_sort[], int n);//array si marimea array-ului
void insertion_sort(int arr_sort[], int n);//array si marimea array-ului
void functie_duplicare_array(int arr_one[], int arr_sec[], int max_index);
int maxim(int array[], int maxIndex);
int minim(int array[], int maxIndex);
int medieAritmetica(int array[], int maxIndex, int maximNrLista);
void metodaSelectareRand(int array[], int maxIndex, int lista[], int secIndex);
int generator() {//generator schimbat de Alex Nistor
	int random = (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	return random * (-1);
}

enum list
{
	ID_Canvas =				wxID_HIGHEST - 1,
	ID_Panou =				wxID_HIGHEST - 2,

	BUTTON_EXIT =			wxID_HIGHEST - 3,
	BUTTON_LOADING =		wxID_HIGHEST - 4,
	BUTTON_BUBBLE =			wxID_HIGHEST - 5,
	BUTTON_INSERTION =		wxID_HIGHEST - 6,

	BUTTON_MIN_MAX =		wxID_HIGHEST - 7,
	BUTTON_AFISARE_SORT =	wxID_HIGHEST - 8,
	BUTTON_BUTON_RAND =		wxID_HIGHEST - 9,
	BUTTON_INSTRUCTIUNI =	wxID_HIGHEST - 10,
};

class MyCanvas;

class RenderTimer : public wxTimer
{
    MyCanvas* pane;
public:
    RenderTimer(MyCanvas* pane);
    void Notify();
    void start();
};


class MyCanvas : public wxPanel
{
public:
	MyCanvas(wxFrame* parent);
	wxBrush brushFundal;//definim 4 brush-uri
	wxBrush brushText;
	wxBrush brushGrafic;
	wxBrush brushGraficSec;

	wxButton* exit_button;//buton exit
	wxButton* loading_button;//buton insert grafic si initializare citire fisier
	wxButton* bubble_sort;//buton functie bubble sort
	wxButton* insertion_sort;//buton functie insertion sort

	wxButton* min_max;
	wxButton* afisare_sort;
	wxButton* num_rand;
	wxButton* instructiuni;

	void paintEvent(wxPaintEvent& evt);
	void paintNow();
	void render(wxDC& dc);
	wxDECLARE_EVENT_TABLE();
};

class MyFrame;

class MyApp: public wxApp
{
    bool OnInit();
    
    MyFrame* frame;
};
IMPLEMENT_APP(MyApp)

class MyFrame : public wxFrame
{
    RenderTimer* timer;
    MyCanvas* drawPane;
    
public:
    MyFrame() : wxFrame((wxFrame *)NULL, -1,  wxT("ProiectGrafice"), wxPoint(50,50), wxSize(1200, 600))
    {
		//little resizing hack :P
		SetMinClientSize(wxSize(1200, 600));//minimum size
		SetMaxClientSize(wxSize(1200, 600));//maximum size

        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        drawPane = new MyCanvas( this );
        sizer->Add(drawPane, 1, wxEXPAND);
        SetSizer(sizer);
		//renderloop , momentan nu avem nevoie

		//timer = new RenderTimer(drawPane);
		//Show();
		//timer->start();
    }
	~MyFrame()//destructor timer
	{
		delete timer;
	};
	void indexare		(wxCommandEvent& evt); //functie pentru butonul 2 , activare functia de indexare
	void exitButon		(wxCommandEvent& evt);
	void bubbleSort		(wxCommandEvent& evt);
	void insertionSort	(wxCommandEvent& evt);

	void minMax			(wxCommandEvent& evt);
	void afisareSort	(wxCommandEvent& evt);
	void numRand		(wxCommandEvent& evt);
	void instructiuni	(wxCommandEvent& evt);

	void onClose		(wxCloseEvent& evt);//functie pentru butonul "Exit"
	DECLARE_EVENT_TABLE();
};

bool MyApp::OnInit()
{
    frame = new MyFrame();
	frame->SetIcons(wxICON(IDI_ICON1));
    frame->Show();
    return true;
} 



BEGIN_EVENT_TABLE(MyCanvas, wxPanel)
EVT_PAINT(MyCanvas::paintEvent)
END_EVENT_TABLE()


//definire parmetri desen
MyCanvas::MyCanvas(wxFrame* parent) :
wxPanel(parent)
{
	wxColour wxCol1(255, 255, 255);//Alb
	wxColour wxCol2(0, 0, 0);//Negru
	wxColour wxCol3(0, 255, 0);//Verde
	wxColour wxCol4(61, 139, 216);//Light Blue
	brushFundal = wxBrush(wxCol1);
	brushText = wxBrush(wxCol2);
	brushGrafic = wxBrush(wxCol3);
	brushGraficSec = wxBrush(wxCol4);
	}

void MyCanvas::paintEvent(wxPaintEvent& evt)
{
    wxPaintDC dc(this);
    render(dc);
}

void MyCanvas::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

void MyCanvas::render(wxDC& dc)
{
	//metoda desen interfata
	dc.SetBrush(brushFundal);
	dc.DrawRectangle(5, 5, 390, 240);	// first window
	dc.DrawRectangle(5, 255, 390, 340);	//second window
	dc.DrawRectangle(405, 5, 790, 590);	// third window
	dc.SetBrush(brushText);
	//dc.DrawText("F1 -> dc.DrawRectangle(5, 5, 390, 240)  // first window", 10, 10);
	//dc.DrawText("F2 -> dc.DrawRectangle(5, 255, 390, 320);	//second window", 10, 260);
	//dc.DrawText("Main F3 -> dc.DrawRectangle(405, 5, 790, 570);	// third window", 410, 10);

	dc.DrawLine(405, 300, 1195, 300); // despartitor Main F3
	dc.DrawLine(400, 0, 400, 600);
	dc.DrawLine(0, 250, 400, 250);

	//butoane dreapta
	exit_button = new wxButton(this, BUTTON_EXIT, _T("Exit"), pozitie_exit_button, marime_buton, 0);      //pozitie_exit_button
	loading_button = new wxButton(this, BUTTON_LOADING, _T("Indexare lista"), pozitie_loading_button, marime_buton, 0); //buton introducere
	bubble_sort = new wxButton(this, BUTTON_BUBBLE, _T("Bubble Sort"), pozitie_bubble_sort, marime_buton, 0); //buton Bubble_sort
	insertion_sort = new wxButton(this, BUTTON_INSERTION, _T("Insertion Sort"), pozitie_insertion_sort, marime_buton, 0); //buton insertion_sort

	//butoane stanga
	min_max = new wxButton(this, BUTTON_MIN_MAX, _T("Minim Maxim"), pozitie_min_max, marime_buton, 0);//Buton Minim Maxim MedieAritmetica
	afisare_sort = new wxButton(this, BUTTON_AFISARE_SORT, _T("ListaSortata"), afisare_numere_sortate, marime_buton, 0);//Buton Lista Sortare
	num_rand = new wxButton(this, BUTTON_BUTON_RAND, _T("SelectareRand"), pozitie_buton_rand, marime_buton, 0);//Buton Selectare Nr. Random
	instructiuni = new wxButton(this, BUTTON_INSTRUCTIUNI, _T("Instructiuni"), pozitie_instructiuni, marime_buton, 0);//Buton Instructiuni

	//metoda desen first_graf
	if (status_graf_one == true) {
		dc.SetBrush(brushText);
		dc.DrawRectangle(430, 30, 740, 240);
		dc.SetBrush(brushGraficSec);
		//functie si int pentru desenarea linilor graficului
		int numar = -50;
		int scadenta = 50;
		for (int lupus = 0; lupus <= 4; lupus++) {
			dc.DrawRectangle(430, 315 - scadenta, 740, 3);
			dc.DrawText(wxString::Format(" %d", numar + scadenta), 405, 310 - scadenta);
			scadenta += 50;
		}

		dc.SetBrush(brushGrafic);
		int add = 0;
		for (int indexor = 0; indexor <= 99; indexor++) {
			dc.DrawRectangle(450 + add, 265, 7, -(lista_Neordonate[indexor]));
			add = add + 7;
		}
	}

	//metoda desen sec_graf
	if (status_graf_sec == true) {
		dc.SetBrush(brushText);
		dc.DrawRectangle(430, 330, 740, 240);
		dc.SetBrush(brushGrafic);
		//functie si int pentru desenarea linilor graficului
		int scadenta = 50;
		int numar = -50;
		for (int lupus = 0; lupus <= 4; lupus++) {
			dc.DrawRectangle(430, 615 - scadenta, 740, 3);
			dc.DrawText(wxString::Format(" %d", numar + scadenta), 405, 610 - scadenta);
			scadenta += 50;
		}

		dc.SetBrush(brushGraficSec);
		int add = 0;
		for (int indexor = 0; indexor <= 99; indexor++) {
			dc.DrawRectangle(450 + add, 565, 7, -(lista_Ordonate[indexor]));
			add = add + 7;
		}
	}

	//void instructiuni(wxCommandEvent & evt);

	if (status_mini_ecran == 'A') {// instructiuni
		dc.DrawText("-->Instructiuni utilizare<--", 135, 330);
		dc.DrawText("*Butonul 'Indexare lista' cauta lista in directorul programului:", 30, 350);
		dc.DrawText("->Lista trebuie se numeasca 'listaNumere.txt' pentru a fi adaugata.", 40, 370);
		dc.DrawText("*Butonul 'Bubble Sort' si 'Insertion Sort' sunt folosite pentru a ", 30, 390);
		dc.DrawText("selecta algoritmul de sortare.", 40, 410);
		dc.DrawText("*Butonul 'Minim Maxim' afiseaza : ->Cel mai mic numar din lista;", 30, 430);
		dc.DrawText("->Cel mai mare numar din lista;", 40, 450);
		dc.DrawText("->Media aritmetica a functiei.", 40, 470);
		dc.DrawText("*Butonul 'Selectare Rand' citeste 3 numere random din lista.", 30, 490);
		dc.DrawText("*Butonul 'Instructiuni' afiseaza acest mesaj.", 40, 510);
		
	}

	if (status_mini_ecran == 'B') {// min-max
		dc.SetBrush(brushText);
		int nrMaxim, nrMinim, nrMedie;
		nrMaxim = maxim(lista_Neordonate, 100);
		nrMinim = minim(lista_Neordonate, 100);
		nrMedie = medieAritmetica(lista_Neordonate, 100, 200);
		dc.DrawText(wxString::Format("*Numarul maxim din lista este: %d", nrMaxim), 40, 350);
		dc.DrawText(wxString::Format("*Numarul minim din lista este: %d", nrMinim), 40, 370);
		dc.DrawText(wxString::Format("*Media aritmetica a listei este este: %d", nrMedie), 40, 390);
	}

	if (status_mini_ecran == 'C') {
		// sort
		int adunare_x = 25, adunare_y = 25, index = 0;
		dc.DrawText("Lista sortata este ", 150, 300);
		for (int z = 0; z <= 9; z++) {
			for (int i = 0; i <= 9; i++) {
				dc.DrawText(wxString::Format(" %d", lista_Ordonate[index]), 40 + adunare_x, 300 + adunare_y);
				
				adunare_x = adunare_x + 25;
				index++;
			}
			adunare_x = 25;
			adunare_y = adunare_y + 25;
		}
	}

	if (status_mini_ecran == 'D') {
		dc.DrawText(wxString::Format("*Primul numar selectat este: %d", lista_request[0]), 40, 350);
		dc.DrawText(wxString::Format("*Al doilea numar selectat este: %d", lista_request[1]), 40, 370);
		dc.DrawText(wxString::Format("*Al treilea numar selectat este: %d", lista_request[2]), 40, 390);
	}
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_CLOSE(MyFrame::onClose)
EVT_BUTTON(BUTTON_EXIT, MyFrame::exitButon)
EVT_BUTTON(BUTTON_LOADING, MyFrame::indexare)
EVT_BUTTON(BUTTON_BUBBLE, MyFrame::bubbleSort)
EVT_BUTTON(BUTTON_INSERTION, MyFrame::insertionSort)

EVT_BUTTON(BUTTON_MIN_MAX, MyFrame::minMax)
EVT_BUTTON(BUTTON_AFISARE_SORT, MyFrame::afisareSort)
EVT_BUTTON(BUTTON_BUTON_RAND, MyFrame::numRand)
EVT_BUTTON(BUTTON_INSTRUCTIUNI, MyFrame::instructiuni)
END_EVENT_TABLE()

/*
____________________________________________
|                                          |
|				 EVENTS;                   |
|                FUNCTII;                  |
|__________________________________________|
*/



RenderTimer::RenderTimer(MyCanvas* pane) : wxTimer()
{
	RenderTimer::pane = pane;
}

void RenderTimer::Notify()
{
	pane->Refresh();
}

void RenderTimer::start()
{
	wxTimer::Start(0);
}

void MyFrame::onClose(wxCloseEvent& evt)
{
	//timer->Stop();
	evt.Skip();
}

void MyFrame::exitButon(wxCommandEvent& evt)
{
	//timer->Stop();
	evt.Skip();
	wxWindow::Close();//pentru inchiderea aplicatiei
}

void MyFrame::indexare(wxCommandEvent& evt)
{
	operatiFisier(99, "listaNumere.txt", lista_Neordonate);
	status_graf_one = true;
	wxWindow::Refresh();
}

void MyFrame::bubbleSort(wxCommandEvent& evt)
{
	functie_duplicare_array(lista_Neordonate, lista_Ordonate, 100);
	bubble_sort(lista_Ordonate, 100);
	status_graf_sec = true;
	wxWindow::Refresh();
}

void MyFrame::insertionSort(wxCommandEvent& evt)
{
	functie_duplicare_array(lista_Neordonate, lista_Ordonate, 100);
	insertion_sort(lista_Ordonate, 100);
	status_graf_sec = true;
	wxWindow::Refresh();
}

void MyFrame::minMax(wxCommandEvent& evt) {
	status_mini_ecran = 'B';
	wxWindow::Refresh();
}

void MyFrame::afisareSort(wxCommandEvent& evt) {
	status_mini_ecran = 'C';
	wxWindow::Refresh();
}

void MyFrame::numRand(wxCommandEvent& evt) {
	status_mini_ecran = 'D';
	//fac procesarea in event handler pentru a fi procesat numai odata,
	//daca as face procesarea in clasa de desen ar fi updatata odata cu update-ul de ecran, cea ce ar aduce niste bugs;
	metodaSelectareRand(lista_Neordonate, 100, lista_request, 2);
	wxWindow::Refresh();
}

void MyFrame::instructiuni(wxCommandEvent& evt) {
	status_mini_ecran = 'A';
	wxWindow::Refresh();
}

void operatiFisier(int i, std::string filename, int lista[]) {//functie pentru indexare dintr-un fisier
	fisier.open(filename);
	int dummy = 0;
	for (int z = 0; z <= i; z++) {
		fisier >> dummy;
		lista[z] = dummy;
	}
	fisier.close();
}
std::string conversie_string(int lista[], int index) {//pentru conversie din array in string, pentru afisare
	std::string dummyString;
	streamString << lista[index];
	streamString >> dummyString;
	return dummyString;
}
void functie_duplicare_array(int arr_one[], int arr_sec[], int max_index) {
	int cd = 0;
	while (cd <= max_index) {
		arr_sec[cd] = arr_one[cd];
		cd++;
	}
}

//algoritmi sortare Raul Mihai
void bubble_sort(int arr_sort[], int n) {
	//array si marimea array-ului
	int aux;
	for (int i = 0; i < (n - 1); i++)//bubble sort
	{
		for (int j = 0; j < (n - i - 1); j++)
		{
			if (arr_sort[j] > arr_sort[j + 1])
			{
				aux = arr_sort[j];
				arr_sort[j] = arr_sort[j + 1];
				arr_sort[j + 1] = aux;
			}
		}
	}
}
void insertion_sort(int arr_sort[], int n) {
	//array si marimea array-ului
	int i = 0, elem = 0, aux = 0, k = 0, j = 0;
	for (i = 1; i < n; i++)
	{
		elem = arr_sort[i];
		if (elem < arr_sort[i - 1])
		{
			for (j = 0; j <= i; j++)
			{
				if (elem < arr_sort[j])
				{
					aux = j;
					for (k = i; k > j; k--)
						arr_sort[k] = arr_sort[k - 1];
					break;
				}
			}
		}
		else
			continue;
		arr_sort[aux] = elem;
	}
}

//parte Anamaria
int maxim(int array[], int maxIndex) {
	int q = 0;
	int numar = array[q];
	int numar_maxim = array[q + 1];
	for (q; q <= maxIndex; q++) {
		if (numar > numar_maxim) {
			numar_maxim = array[q];
		}
		else {
			numar = numar_maxim;
			numar_maxim = array[q];
		}
	}
	return numar;
}
int minim(int array[], int maxIndex) {
	int i = 0, numar = array[i];
	int numar_minim = array[i + 1];
	for (int i = 0; i <= maxIndex; i++) {
		if (numar < numar_minim) {
			numar_minim = array[i];
		}
		else {
			numar = numar_minim;
			numar_minim = array[i];
		}
	}
	return numar;
}
int medieAritmetica(int array[], int maxIndex, int maximNrLista) {
	int numar = 0;
	int medieAritmetica;
	for (int d = 0; d <= 99; d++) {
		numar = numar + array[d];
	}
	medieAritmetica = numar / (maximNrLista / 2);
	return medieAritmetica;
}

//metoda Stefan Mitrea
void metodaSelectareRand(int array[], int maxIndex, int lista[], int secIndex) {//min 3
	srand(generator());// seed schimbat de Alex Nistor
	for (int z = 0; z <= secIndex; z++) {
		lista[z] = array[rand() % maxIndex];
	}
}