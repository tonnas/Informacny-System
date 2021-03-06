#include "Velkosklad.h"
#include "PriorityQueue_Heap.h"
#include <map>
#include <iostream>

using namespace std;
using namespace DS;

Velkosklad::Velkosklad() :
	sklad_(new ArrayList<Polozka*>()),
	dodavky_(new LinkedList<Dodavka*>()),
	objednavky_(new ArrayList<Objednavka*>()),
	autoPrevoz_(new Stack<Objednavka*>())
{
}

Velkosklad::Velkosklad(const Velkosklad & dalsiVelkosklad) :
	sklad_(new ArrayList<Polozka*>(*dalsiVelkosklad.sklad_)),
	dodavky_(new LinkedList<Dodavka*>(*dalsiVelkosklad.dodavky_)),
	objednavky_(new ArrayList<Objednavka*>(*dalsiVelkosklad.objednavky_)),
	autoPrevoz_(new Stack<Objednavka*>(*dalsiVelkosklad.autoPrevoz_))
{	
}

Velkosklad::~Velkosklad()
{		   
	delete dodavky_;
	vyprazdnitAuto();
	delete autoPrevoz_;
	cout << objednavky_->size();
	vymazZoZoznamu<Objednavka*>(*objednavky_);
	delete objednavky_;
	vymazZoZoznamu<Polozka*>(*sklad_);
	delete sklad_;
}

bool Velkosklad::zaevidujObjednavku(Objednavka * pObjednavka)
{
	objednavky_->add(pObjednavka);
	return true;
}

bool Velkosklad::zaevidujDodavku(Dodavka * pDodavka)
{
	dodavky_->add(pDodavka);
	return true;
}

void Velkosklad::odovzdajZakanikovy()
{
	if (autoPrevoz_->isEmpty())
	{
		cout << "$ Auto je prazdne" << endl;
		return;
	}
	cout << "$ Objednavka";
	autoPrevoz_->pop()->oznacOdoslana();
	cout << " ODOVZDANA zakaznikovi " << endl;
}

void Velkosklad::vyskladnenie(int datum)
{
	if (!autoPrevoz_->isEmpty())
	{
		cerr << "~ Auto este nie je prazdne " << endl;
		return;
	}
	zaskladni();
	bool preskoc = false;
	int mnozstvo = 0;
	PriorityQueue_Heap<Objednavka*>* zoradenie = new PriorityQueue_Heap<Objednavka*>();
	for (auto objednavka : *objednavky_)
	{
		if (objednavka->dajDatumDorucenia() == datum + 1 
			&& objednavka->jeCakajuca())
		{
			if (skontrolujMnozstvoVsklade(objednavka)) continue;
			pripravVyskladnenieUpravDodavky(objednavka);
			zoradenie->push(objednavka->dajPredajnu().dajZona(), objednavka);
			objednavka->oznacVyexpedovanu();
		}
 	}
	if (zoradenie->size() == 0)
	{
		cout << "$ Dnes nie je ziadny tovar na vyskladnenie " << endl;
		return;
	}
	while(!zoradenie->isEmpty())
	{
		autoPrevoz_->push(zoradenie->pop());
	}
	delete zoradenie;
	cout << "$ Vsetky platne objednavky odpisane zo skladu. \n  Tovar nalozeny do auta." << endl;
}
 
void Velkosklad::vyhladanieOdberatelaVody(const string & voda, int odkedy, int dokedy)
{
	map<string, int> zoznamMap;
	for (auto objednavka : *objednavky_)
	{
		if ((objednavka->dajDatumDorucenia() >= dokedy
			&& objednavka->dajDatumDorucenia() <= dokedy)
			&& objednavka->jeOdoslana());
		{
			for (auto polozka : *objednavka->dajPolozky())
			{
				if (polozka->dajMineralku().dajEAN() == voda)
				{
					string zakaznik = objednavka->dajPredajnu().dajMenoZakaznika();
					if (zoznamMap.count(zakaznik) == 0)
					{
						zoznamMap[objednavka->dajPredajnu().dajMenoZakaznika()] = polozka->dajMnozstvo();
					}
					else
					{
						zoznamMap[objednavka->dajPredajnu().dajMenoZakaznika()] += polozka->dajMnozstvo();
					}
				}
			}
		}
	}
	string maxZakaznik = "";
	int maxMnozstvo = 0;
	for (auto prvok : zoznamMap)
	{
		if (prvok.second > maxMnozstvo)
		{
			maxMnozstvo = prvok.second;
			maxZakaznik = prvok.first;
		}
	}
	if (maxZakaznik == "")
	{
		cout << "$ Nenasiel sa ziadny odberatel" << endl;
		return;
	}
	cout << "$ Zakaznik, ktory odobral najvacsie mnozstvo za dane obdobie:  (podla min.vody)\n        - "
		 << maxZakaznik << " - " << maxMnozstvo << " ks " << endl;
}

void Velkosklad::vyhladanieOdberatelaOdDodavatela(const string & dodvatel, int odkedy, int dokedy)
{
	map<string, int> zoznamMap;
	for (auto objednavka : *objednavky_)
	{
		if ((objednavka->dajDatumDorucenia() >= dokedy
			&& objednavka->dajDatumDorucenia() <= dokedy)
			&& objednavka->jeOdoslana())
		{
			for (auto polozka : *objednavka->dajPolozky())
			{
				if (polozka->dajMineralku().dajNazovDodavatela() == dodvatel)
				{
					string zakaznik = objednavka->dajPredajnu().dajMenoZakaznika();
					if (zoznamMap.count(zakaznik) == 0)
					{
						zoznamMap[objednavka->dajPredajnu().dajMenoZakaznika()] = polozka->dajMnozstvo();
					}
					else
					{
						zoznamMap[objednavka->dajPredajnu().dajMenoZakaznika()] += polozka->dajMnozstvo();
					}
				}
			}
		}
	}
	string maxZakaznik = "";
	int maxMnozstvo = 0;
	for (auto prvok : zoznamMap)
	{
		if (prvok.second > maxMnozstvo)
		{
			maxMnozstvo = prvok.second;
			maxZakaznik = prvok.first;
		}
	}
	if (maxZakaznik == "")
	{
		cout << "$ Nenasiel sa ziadny odberatel" << endl;
		return;
	}
	cout << "$ Zakaznik, ktory odobral najvacsie mnozstvo za dane obdobie:  (podla odberatela)\n        - "
		<< maxZakaznik << " - " << maxMnozstvo << " ks " << endl;
}

void Velkosklad::vyhladanieDodavatela(int odkedy, int dokedy)
{
	ArrayList<Polozka*>* pozadovanePolozky = new ArrayList<Polozka*>();
	for (auto objednavka : *objednavky_)
	{
		if (objednavka->dajDatumDorucenia() >= dokedy
			&& objednavka->dajDatumDorucenia() <= dokedy) continue;
		{
			for (auto polozka : *objednavka->dajPolozky())
			{
				Polozka* pom = new Polozka(polozka->dajMineralku(), polozka->dajMnozstvo());
				pozadovanePolozky->add(pom);
			}
		}
	}
	triedeniePoloziek(*pozadovanePolozky);
	Polozka* maxim = NULL;
	int maxMnozstvo = 0;
	for (auto polozkaTriedena : *pozadovanePolozky)
	{
		if (polozkaTriedena->dajMnozstvo() > maxMnozstvo)
		{
			maxim = polozkaTriedena;								  
			maxMnozstvo = maxim->dajMnozstvo();
		}
	}
	if (maxim == NULL)
	{
		return;
	}
	cout << "$ Najziadanejsi dodavatel za dane obdobie: \n        - ";
	cout << maxim->dajMineralku().dajNazovDodavatela() << " - " << maxim->dajMnozstvo() << " ks" << endl;
	vymazZoZoznamu<Polozka*>(*pozadovanePolozky);
	delete pozadovanePolozky;
}

void Velkosklad::triedeniePoloziek(ArrayList<Polozka*>& polozky)
{
	for (auto polozkaTriedena1 : polozky)
	{
		if (polozkaTriedena1->dajMnozstvo() == 0) continue;
		for (auto polozkaTriedena2 : polozky)
		{
			if (polozkaTriedena2->dajMnozstvo() == 0) continue;
			if (polozkaTriedena1->dajMineralku().dajNazov() == polozkaTriedena2->dajMineralku().dajNazov()
				&& polozkaTriedena1 != polozkaTriedena2)
			{
				polozkaTriedena1->zvysMnozstvo(polozkaTriedena2->dajMnozstvo());
				polozkaTriedena2->znizMnozstvo(polozkaTriedena2->dajMnozstvo());
			}
		}
	}
}

void Velkosklad::kontrolaPoziadaviek()
{
	zaskladni();
	string pom;
	cout << "$ Chybajuce pozadovane mineralne vody: " << endl;
	ArrayList<Polozka*>* pozadovanePolozky = new ArrayList<Polozka*>();
	ArrayList<string>* vypisZoradenie = new ArrayList<string>();
	for (auto objednavka : *objednavky_)
	{
		if (!objednavka->jeCakajuca()) continue;
		for (auto polozka : *objednavka->dajPolozky())
		{
			Polozka* pom = new Polozka(polozka->dajMineralku(), polozka->dajMnozstvo());
			pozadovanePolozky->add(pom);
		}
	}
	if (pozadovanePolozky->size() == 0)
	{
		cout << "        - Nechybaju ziadne vody\n" << endl;
		return;
	}
	triedeniePoloziek(*pozadovanePolozky);
	
	for (auto poz : *pozadovanePolozky)
	{
		if (poz->dajMnozstvo() == 0) continue;
		for (auto polS : *sklad_)
		{
			if (polS->dajMineralku().dajNazov() == poz->dajMineralku().dajNazov())
			{
				poz->znizMnozstvo(polS->dajMnozstvo());
			}
		}								   			
	}
	for (auto polozkaVypis : *pozadovanePolozky)
	{
		if (polozkaVypis->dajMnozstvo() > 0)
		{
			pom = "";
			pom.append(polozkaVypis->dajMineralku().dajNazovDodavatela());
			pom.append(" - ");
			pom.append(polozkaVypis->dajMineralku().dajNazov());
			pom.append(" - ");
			pom.append(to_string(polozkaVypis->dajMnozstvo()));
			pom.append(" ks ");
			vypisZoradenie->add(pom);
		}
	}	
	zoradPole(*vypisZoradenie);
	for (auto vyp : *vypisZoradenie)	
	{
		cout << "        - " << vyp << endl;
	}
	vymazZoZoznamu<Polozka*>(*pozadovanePolozky);
	delete vypisZoradenie;
	delete pozadovanePolozky;
}

void Velkosklad::zoradDodavky()
{
	PriorityQueue_Heap<Dodavka*>*  zoradenie = new PriorityQueue_Heap<Dodavka*>();
	for (auto dodavka : *dodavky_)
	{
		Dodavka* nova = new Dodavka(*dodavka->dajMinetralku(), dodavka->dajMnozstvo(), dodavka->dajDatumPlnenia());
		zoradenie->push(nova->dajDatumPlnenia(), nova);
	}
	dodavky_->clear();
	while (!zoradenie->isEmpty())
	{
		dodavky_->add(zoradenie->pop());
	}					   
	delete zoradenie;
	
}

void Velkosklad::zaskladni()
{
	if (sklad_->size() != 0)
	{
		vymazZoZoznamu<Polozka*>(*sklad_);
	}
	zoradDodavky();
	bool preskoc = false;
	for (auto dodavka : *dodavky_)
	{   
		for (auto polozka : *sklad_)
		{
			if (polozka->dajMineralku().dajNazov() == dodavka->dajMinetralku()->dajNazov())
			{
				polozka->zvysMnozstvo(dodavka->dajMnozstvo());
				preskoc = true;
				break;
			}
		}
		if (!preskoc)
		{
			Polozka* novaPolozka = new Polozka(*dodavka->dajMinetralku(), dodavka->dajMnozstvo());
			sklad_->add(novaPolozka);
		}
		preskoc = false;
	}	 
}

void Velkosklad::vypisNeplatneObjednavky()
{
	cout << "$ Neplatne objednavky: " << endl;
	for (auto objednavka : *objednavky_)
	{
		if (objednavka->dajStavObjednavky() == "NEPLATNA")
		{
			cout << "       ";
			cout << objednavka->vypisObjednavku() << endl;
		}		   		
	}	
	cout << endl;
}

void Velkosklad::vypisVsetkyObjednavky()
{
	cout << "Vsetky objednavky :" << endl;
	for (auto objednavka : *objednavky_)
	{
		cout << objednavka->vypisObjednavku() << endl;
	}
}

void Velkosklad::vypisSklad()
{
	zaskladni();
	ArrayList<string>* vypisZoradenie = new ArrayList<string>();
	string pom;
	cout << "$ Obsah skladu je:" << endl;
	for (auto polozka : *sklad_)
	{
		pom = "";
		pom.append(polozka->dajMineralku().dajNazovDodavatela());
		pom.append(" - ");
		pom.append(polozka->dajMineralku().dajNazov());
		pom.append(" - ");
		pom.append(to_string(polozka->dajMnozstvo()));
		pom.append(" ks  \n");
		vypisZoradenie->add(pom);
	}
	zoradPole(*vypisZoradenie);
	for (auto vypis : *vypisZoradenie)
	{
		cout << "	- " << vypis;
	}
	delete vypisZoradenie;
}

bool Velkosklad::skontrolujMnozstvoVsklade(Objednavka * objednavka)
{
	for (auto polozka : *objednavka->dajPolozky())
	{
		for (auto sklad : *sklad_)
		{
			if (polozka->dajMineralku().dajNazov() == sklad->dajMineralku().dajNazov()
				&& (sklad->dajMnozstvo() - polozka->dajMnozstvo() >= 0))
			{
				break;
			}
			else if (polozka->dajMineralku().dajNazov() == sklad->dajMineralku().dajNazov()
				&& (sklad->dajMnozstvo() - polozka->dajMnozstvo() < 0))
			{
				objednavka->oznacNeplatnu();
				return true;
			}
		}
	}
	return false;
}

void Velkosklad::pripravVyskladnenieUpravDodavky(Objednavka * objednavka)
{
	int mnozstvo = 0;
	for (auto polozka : *objednavka->dajPolozky())
	{
		mnozstvo = polozka->dajMnozstvo();
		for (auto dodavka : *dodavky_)
		{
			if (polozka->dajMineralku().dajNazov() == dodavka->dajMinetralku()->dajNazov()
				&& dodavka->dajMnozstvo() - mnozstvo > 0)
			{
				dodavka->znizMnozstvo(mnozstvo);
				break;
			}
			else if (polozka->dajMineralku().dajNazov() == dodavka->dajMinetralku()->dajNazov()
					&& dodavka->dajMnozstvo() - mnozstvo <= 0)
			{
				dodavka->znizMnozstvo(dodavka->dajMnozstvo());
				mnozstvo -= dodavka->dajMnozstvo();
			}
			if (dodavka->dajMnozstvo() == 0)
			{
				dodavky_->tryRemove(dodavka);
			}
		}
	}
}

void Velkosklad::zoradPole(DS::ArrayList<string>& pole)
{
	bool vymeneny = true;
	while (vymeneny)
	{
		vymeneny = false;
		for (int i = 0; i < static_cast<int>(pole.size() - 1); i++)
		{
			if (pole[i] > pole[i + 1])											 
			{
				string pom = pole[i];
				pole[i] = pole[i + 1];
				pole[i + 1] = pom;
				vymeneny = true;
			}
		}
	}
}

void Velkosklad::vyprazdnitAuto()
{
	while (!autoPrevoz_->isEmpty())
	{
		autoPrevoz_->pop();
	}
}

template <typename T>			  
void Velkosklad::vymazZoZoznamu(DS::ArrayList<T>& zoznam)
{
	for (auto prvok : zoznam)
	{
		delete prvok;
	}
	zoznam.clear();
}

void Velkosklad::ulozDoSuboru(std::ostream& subor)
{
	for (auto dodavka : *dodavky_)
	{
		subor << dodavka->dajPrikazNaUlozenie() << endl;
	}

	for (auto objednavka : *objednavky_)
	{
		subor << objednavka->dajPrikazNaUlozenie() << endl;
	}
}