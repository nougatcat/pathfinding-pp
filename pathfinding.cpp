
#include <vector>
#include <iostream>
#include <queue>
#include <fstream>

#define INFINITY 10000 // условное число, обозначающее бесконечность.

using namespace std;
enum vert
{
	nopath = -1,
	notseen,
	seen,
	done,
};

class GraphClass
{
private:
	vector<vector<int>> matrix;
	size_t vertex; //количество вершин

	//этот метод используется в поиске максимального потока (алгоритм Эдмондса-Карпа). Пользователю он не должен быть доступен
	int FindPath(vector<vector<int> >& f, int source, int target) // source - из этой вершины начинаем, target - в эту вершину идем
	{ //f создаем как изначально пустую матрицу в основном методе
		int CurVertex; //текущая вершина
		// Flow - значение потока через данную вершину на данном шаге поиска
		// Link[i] хранит номер предыдущей вешины на пути i -> источник, линк поможет восстановить маршрут, то есть в i-м месте этого вектора находится номер вершины, из которой мы пришли в i-ую
		vector<int> Flow; Flow.resize(vertex, 0);  //resize - это заполнение. Первый аргумент -сколько, второй -чего										
		vector<int> Link; Link.resize(vertex, -1);

		Flow[source] = INFINITY;
		//поиск пути через поиск в ширину
		queue<int> q;
		q.push(source);
		while (!q.empty())
		{
			CurVertex = q.front();
			q.pop(); //в очереди - поп из начала

			for (int i = 0; i < vertex; i++)
			{
				if (matrix[CurVertex][i] - f[CurVertex][i] > 0 && Flow[i] == 0)
				{
					q.push(i); //в i-ую вершину мы попадаем через вершину curvertex
					Link[i] = CurVertex;

					/*
					if (matrix[CurVertex][i] - f[CurVertex][i] < Flow[CurVertex]) 
						Flow[i] = matrix[CurVertex][i] - f[CurVertex][i];
					else 
						Flow[i] = Flow[CurVertex];
						*/	//if-else меняем на тернарный оператор
					(Flow[CurVertex] - (matrix[CurVertex][i] - f[CurVertex][i])) > 0 ?
						(Flow[i] = matrix[CurVertex][i] - f[CurVertex][i])
						:
						(Flow[i] = Flow[CurVertex]);
				}
			}
		}

		if (Link[target] == -1) return 0; //если не удалось добраться до финального пункта, то ретурн 0
		CurVertex = target;

		vector<int> OnRecord; //запись пути для вывода на консоль
		OnRecord.push_back(CurVertex);

		while (CurVertex != source)
		{
			f[Link[CurVertex]][CurVertex] += Flow[target];
			CurVertex = Link[CurVertex]; //сохраняем информацию о том, сколько "груза" провозится

			OnRecord.push_back(CurVertex);
		}
		
		cout << "\nПоток " << Flow[target] << " Путь: ";
		//путь, по которому груз перемещался:
		for (vector<int>::reverse_iterator i = OnRecord.rbegin(); i != OnRecord.rend(); i++) //reverse iterator
		{
			cout << *i << " ";
		}
		return Flow[target];
	}

public:
	GraphClass(vector<vector<int>> matrix) //Этот конструктор принимает матрицу
	{
		this->matrix = matrix;
		vertex = matrix.size();
	}

	GraphClass(const size_t vertex) //Этот конструктор создает пустую 2д матрицу vertex элементов. Не нужно заполнять неравное количество строк и столбцов. Для графа это бессмыссленно, поэтому на вход только одно число
	{
		// Заполнение
		matrix.resize(vertex, vector<int>(vertex, 0));
		this->vertex = vertex;
	}
	GraphClass() //Это конструктор по умолчанию
	{
		// Заполнение
		vertex = 4; 
		matrix.resize(vertex, vector<int>(vertex, 0));
	}

	void BFS()  //поиск в ширину на графе. Смотрит расстояние от 0 вершины до всех остальных
	{
		vector<int> used(vertex, notseen);
		vector<int> dist(vertex, nopath); //-1 (nopath) - некорректное расстояние
		dist[0] = 0;				      //дист считает расстояние по ребрам
		//BFS//
		queue<int> q;
		int Vertex = 0;
		q.push(0);
		while (!q.empty())
		{
			Vertex = q.front();
			q.pop();
			for (int i = 0; i < vertex; i++)
			{
				if (matrix[Vertex][i] > 0 && used[i] < done)
				{
					q.push(i); //запушит только те вершины, которые имеют связь с vertex
					used[i] = seen;

					if (dist[i] < 0)
						dist[i] = dist[Vertex] + 1;
				}
			}
			used[Vertex] = done;
		}
		
		cout << "\nПосещённые вершины. 2 - посещена, 0 - не посещена "; // выведет, какие вершины обработались
		for (int i = 0; i < vertex; i++) cout << used[i] << " ";

		cout << "\nРасстояние от точки 0 до всех остальных , -1 - нет доступа к вершине "; //дистанция от вершины 0 к остальным вершинам
		for (int i = 0; i < vertex; i++) cout << dist[i] << " ";
		
	}

	inline int Lee(int source, int target) //волновой алгоритм Ли
	{
		vector<vector<int> > f;
		f.resize(vertex, vector<int>(vertex, 0));
		return FindPath(f, source, target);
	}



	//Макс поток (Эдмондс-Карп)
	inline int MaxFlow(int source, int target) // source - откуда идем, target - куда идем
	{	//матрица matrix имеет пропускные способности
		//матрица f - сколько по транспортной системе запущено грузов (сколько по всему пути может пройти грузов). Изначально пустая, заполняется в несколько проходов
		vector<vector<int> > f;
		f.resize(vertex, vector<int>(vertex, 0));

		int MaxFlow = 0;
		int AddFlow = 0;
		do
		{
			AddFlow = FindPath(f, source, target);
			MaxFlow += AddFlow;
		} while (AddFlow > 0); //цикл закончится когда закончатся возможные пути
		return MaxFlow;
	}

	friend ostream& operator<< (ostream& out, GraphClass& graph);
	friend istream& operator>> (istream& in, GraphClass& graph);

};


ostream& operator<< (ostream& out, GraphClass& graph)
{
	out << graph.vertex << endl; //первой строчкой будет количество вершин
	for (size_t i = 0; i < graph.vertex; ++i)
	{
		for (size_t j = 0; j < graph.vertex; ++j)
		{
			out << graph.matrix[i][j] << " ";
		}
		out << "\n";
	}
	return out;
}
istream& operator>> (istream& in, GraphClass& graph) //ввод матрицы или количества вершин. Метод правильно работает с консолью и с правильно сохраненным (с помощью перегруженного <<) файлом
{											
	//из первой строки файла (консоли) должен считать количество вершин
	//из остальных - строки вектора
	//при чтении из файла по умолчанию разделитель - это либо пробел либо переход на новую строку
	int vrtx = -1;
	cout << "Предупреждение! Если используете ввод из файла, то первой строкой должно быть количество вершин графа, а количество строк и столбцов матрицы смежности должно быть одинаково\n";
	cout << "Пример файла:\n" << "2\n" << "1 2\n" << "0 1\n";
	cout << "Количество вершин: ";
	while (vrtx < 0)
	{
		in >> vrtx;
		if (vrtx >= 0)
		{
			graph.matrix.clear();
			graph.vertex = vrtx;
			for (size_t i = 0; i < graph.vertex; ++i)
			{
				vector<int> temp;
				for (size_t j = 0; j < graph.vertex; ++j)
				{
					in >> vrtx; //заново использую ту же переменную, что была для кол-ва вершин, но для задания значений вершин вручную
					temp.push_back(vrtx);
				}
				graph.matrix.push_back(temp);
				cout << "Cледующая строка\n";
			}
		}
		else
		{
			cout << "Неверное количество вершин\n";
			break;
		}
	}
	return in;
}



int main()
{
	setlocale(LC_ALL, "ru");


	GraphClass test2;
	cout << test2 << endl;
	cin >> test2;
	cout << endl;
	cout << test2 << endl;


	vector<vector<int>> mat =
	{
		//  [ 0  1  2  3  4  5  6  7  8  9]	- вершины
			{ 0, 1, 1, 0, 0, 0, 1, 0, 0, 0 },// 0	 // матрица смежности (это граф)
			{ 1, 0, 1, 1, 0, 0, 0, 0, 0, 0 },// 1	 //1 и 0 - связана или не связана i-ая и j-ая вершина с помощью ребра
			{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 },// 2	 //вместо 1 и 0 может стоять вес (стоимость прохода по ребру)
			{ 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },// 3	 //эта матрица симметрична, но для ор.графа она не симметрична
			{ 0, 0, 0, 1, 0, 1, 0, 0, 0, 0 },// 4
			{ 0, 0, 0, 0, 1, 0, 1, 0, 0, 0 },// 5
			{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0 },// 6
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },// 7
			{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },// 8
			{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 } // 9
			//7 8 9 соединены только между собой
	};

	GraphClass test4(mat);
	cout << "Этот граф:\n" << test4;
	cout << "Его размер отдельно:\n" << mat.size();

	cout << endl;
	test4.BFS();
	cout << endl;
	test4.Lee(2, 5);


	cout << endl; cout << endl; cout << endl;


	/////////////////ЭДМОНДС КАРП
	vector<vector<int> > c =
	{
	{	0, 16, 0, 0, 13, 0 },
	{	0, 0, 12, 0, 6, 0 },
	{	0, 0, 0, 0, 9, 20 },
	{	0, 0, 7, 0, 0, 4 },
	{	0, 0, 0, 14, 0, 0 },
	{	0, 0, 0, 0, 0, 0 },
	};

	GraphClass Karp(c);
	cout << "\nРезультат: " << Karp.MaxFlow(0, 5);

	/*
	cout << "//////ПРОВЕРКА БЫСТРОДЕЙСТВИЯ\n";
	clock_t t1 = clock();
	for (int i = 0; i < 1000; i++)
	{
		Karp.MaxFlow(0, 5);
	}
	clock_t t2 = clock();
	cout << (t2 - t1 + .0) / CLOCKS_PER_SEC << endl;
	cout << "////////////////////////\n";
	*/

	/*
	cout << "\nПроверяем поток вывода\n";
	string path = "myfile.txt";
	cout << "Введите путь (название) файла: ";
	cin >> path;
	ofstream fout;
	fout.open(path);
	if (fout.is_open())
	{
		fout << Karp;
		cout << "success!";
	}
	else
	{
		cout << "stream is not open";
	}
	fout.close();
	*/

	/*
	cout << "\nПроверяем поток ввода\n";
	GraphClass Karr;
	string path = "myfile.txt";
	cout << "Введите путь (название) файла: ";
	cin >> path;
	ifstream fin;
	fin.open(path);
	if (fin.is_open())
	{
		fin >> Karr;
		cout << "success!\n";
	}
	else
	{
		cout << "stream is not open\n";
	}
	fin.close();
	cout << Karr;
	*/

	system("pause");
	return 0;
}
