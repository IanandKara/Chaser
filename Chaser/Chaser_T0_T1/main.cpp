#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

class Point // Базовый класс - точка, хранит координаты и вектора скорости объектов
{
public:
	double Sx, Sy, Vx, Vy;
public:
	virtual void UpdateParameters() = 0;
};

class Chaser : public Point // Дочерний класс - преследователь
{
public:
	double V; // скорость преследователя, получаемая от отладчика
public:
	void SetSpeed()
	{
		cin >> V;
	}
	void UpdateParameters()
	{
		cin >> Sx >> Sy >> Vx >> Vy;
	}
};

class Target : public Point			// дочерний класс - цель
{
public:
	bool IsChased;					// поймана ли цель
	double TimeChase, AngleChase;	// время преследования цели и угол преследования
	double AimX, AimY;				// координаты столкновения с преследователем
	static int CountTargets;		// кол-во целей
	static int CountChased;			// кол-во пойманых целей
	static int ChasedAll;			// пойманы ли все цели
public:
	void UpdateParameters()
	{
		cin >> Sx >> Sy >> Vx >> Vy >> IsChased;
		if (IsChased == 1)
		{
			CountChased++;
		}
	}

	// вычисление параметров (TimeChase, AngleChase) для "охоты" на упреждение
	// т.е. преследователь не гонится за целью, а наперед просчитывает оптимальную точку столкновения
	// для примера, уравение кинематики для оси Х: SxП + VxП * t = SxЦ + VxЦ * t
	// но поскольку неизвестных два (скорость преследователя для каждоый оси и t), то преобразуем все в квадратное уравнение и находим t
	void CalculateChaseParameters(double SxChaser, double SyChaser, double VChaser)
	{
		double Dx = Sx - SxChaser;
		double Dy = Sy - SyChaser;

		double A = (Vx * Vx + Vy * Vy) - (VChaser * VChaser);
		double B = 2 * (Dx * Vx + Dy * Vy);
		double C = Dx * Dx + Dy * Dy;

		double Discr = B * B - 4 * A * C;
		double sqrtD = sqrt(Discr);

		double t1 = (-B + sqrtD) / (2 * A);
		double t2 = (-B - sqrtD) / (2 * A);

		if (t1 > 0 && t2 > 0)
		{
			if (t1 < t2)
			{
				TimeChase = t1;
			}
			else
			{
				TimeChase = t2;
			}
		}
		else if (t1 > 0)
		{
			TimeChase = t1;
		}
		else if (t2 > 0)
		{
			TimeChase = t2;
		}
		else
		{
			TimeChase = sqrt(Dx * Dx + Dy * Dy) / VChaser;
		}

		AimX = Sx + Vx * TimeChase;								// точка столкновения по оси Х
		AimY = Sy + Vy * TimeChase;								// точка столкновения по оси Y
		AngleChase = atan2(AimY - SyChaser, AimX - SxChaser);	// вычисление угла для попадания в точку столкновения
	}
};
// инициализация статических переменных
int Target::CountTargets = 0;
int Target::CountChased = 0;
int Target::ChasedAll = 0;

int main()
{
	double t;	// время моделирования // пока непонятно как его использовать, если честно
	double TotalTime = 0; // время поимки всех целей
	double BestTotalTime; // кратчайшее время поимки всех целей, присвиваем 1 миллиард для корректного сравнения с временем первого полного пути
	double BufferXChaser, BufferYChaser; // буферные координаты для запоминания положения преследователя
	double BufferXTarget, BufferYTarget; // буферные координаты для запоминания положения целей
	int i;

	//получение начальных данных от отладчика
	Chaser* chaser = new Chaser;	// преследователь
	chaser->SetSpeed();				// установка скорости преследователю

	cin >> Target::CountTargets;						// кол-во целей
	Target* targets = new Target[Target::CountTargets]; // массив целей
	vector <Target*> CombinationTargets;				// массив указателей на цели для полного перебора через next_permutation из <algorithm>
	vector <Target*> BestCombinationTargets;
	//Target* CurrentTarget;							// указатель на выбранную цель для преследования

	//начало моделирования
	while (Target::ChasedAll == 0)
	{
		// обнуления
		CombinationTargets.clear();
		BestCombinationTargets.clear();
		BestTotalTime = 1e9;
		Target::CountChased = 0;
		//CurrentTarget = nullptr;

		// обновление параметров моделирования
		cin >> t;
		chaser->UpdateParameters(); BufferXChaser = chaser->Sx; BufferYChaser = chaser->Sy;
		for (i = 0; i < Target::CountTargets; i++)
		{
			targets[i].UpdateParameters();

		}

		//заполнение массива для полного перебора непосещенных целей
		for (i = 0; i < Target::CountTargets; i++)
		{
			if (targets[i].IsChased == false)
			{
				CombinationTargets.push_back(&targets[i]);
			}
		}

		// полный перебор вариантов полного пути преследователя и выбор оптимального
		do {
			// расчеты для данной комбинации пути
			for (i = 0; i < CombinationTargets.size(); i++)
			{
				CombinationTargets[i]->CalculateChaseParameters(chaser->Sx, chaser->Sy, chaser->V);
				if (i != 0)
				{																							// это для возвращения цели на место
					CombinationTargets[i]->Sx = BufferXTarget; CombinationTargets[i]->Sy = BufferYTarget;	// поскольку дальше в цикле цель i+1 смещается на время погони цели i
				}																							// и на следующей итерации цель, которая была i+1 становится i и возвращается на место благодаря буферу
				TotalTime = TotalTime + CombinationTargets[i]->TimeChase;									// для вычисления общего времени пути
				chaser->Sx = CombinationTargets[i]->AimX; chaser->Sy = CombinationTargets[i]->AimY;			// якобы преследователь догнал цель, чтобы дальше вычисления велись от этих координат
				if (i < CombinationTargets.size() - 1)
				{
					BufferXTarget = CombinationTargets[i + 1]->Sx; BufferYTarget = CombinationTargets[i + 1]->Sy;				// запись в буфер координат цели i+1
					CombinationTargets[i + 1]->Sx = CombinationTargets[i + 1]->Sx + CombinationTargets[i + 1]->Vx * TotalTime;	// сдвиг координаты x цели i + 1 по общему времени погони, для моделирования погони для данной комбинации пути 
					CombinationTargets[i + 1]->Sy = CombinationTargets[i + 1]->Sy + CombinationTargets[i + 1]->Vy * TotalTime;	// сдвиг координаты н цели i + 1 по общему времени погони
				}
			}
			// определение лучшей комбинации пути
			if (BestTotalTime > TotalTime)
			{
				BestTotalTime = TotalTime;
				BestCombinationTargets = CombinationTargets;
			}
			TotalTime = 0; // сброс для вычисления времени для следующей комбинации
			chaser->Sx = BufferXChaser; chaser->Sy = BufferYChaser; // возвращаем преследователя на его место

		} while (next_permutation(CombinationTargets.begin(), CombinationTargets.end()));	// Переставляет элементы массива местами (Пр: 123, 132, 213, 231...) 

		// условие завершения моделирования, если все пойманы выдача в отладчик 1 (завершение программы), иначе продолжаем погоню
		if (Target::CountChased == Target::CountTargets)
		{
			cout << 0 << endl; //по протоколу взаимодействия нужно выдать угол, но уже все цели посещены, поэтому просто 0
			Target::ChasedAll = 1;
		}
		else
		{
			BestCombinationTargets[0]->CalculateChaseParameters(chaser->Sx, chaser->Sy, chaser->V);
			cout << BestCombinationTargets[0]->AngleChase << endl;
		}
		cout << Target::ChasedAll << endl;
	}

	// очистка памяти
	delete chaser;
	delete[] targets;

	return 0;
}