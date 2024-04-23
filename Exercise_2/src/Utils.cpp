#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


namespace PolygonalLibrary {
bool ImportMesh(const string& filepath,
                PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;
        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        // Test:
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            const vector<unsigned int>& edges = mesh.Cell2DEdges[c];

            for(unsigned int e = 0; e < edges.size() ; e++) // hanno dimesione variabile i vettori, sfrutto size()
            {
                const unsigned int origin = mesh.Cell1DVertices[edges[e]][0];
                const unsigned int end = mesh.Cell1DVertices[edges[e]][1];

                auto findOrigin = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), origin);
                if(findOrigin == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 2;
                }

                auto findEnd = find(mesh.Cell2DVertices[c].begin(), mesh.Cell2DVertices[c].end(), end);
                if(findEnd == mesh.Cell2DVertices[c].end())
                {
                    cerr << "Wrong mesh" << endl;
                    return 3;
                }

            }
        }

    }

    return true;

}
// ***************************************************************************
bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front(); // rimuovere l'elemento in testa alla lista e quindi ridurre la lunghezza di uno

    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        char puntoVirgola; // in questo esercizio i dati sono in colonne diverse, quindi sono separati da un punto e virgola

        unsigned int marker;
        Vector2d coord;

        converter >>  id >> puntoVirgola >> marker >> puntoVirgola >> coord(0) >> puntoVirgola >> coord(1);

        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coord);

        if( marker != 0)
        {
            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }

    }
    file.close();
    return true;
}
// ***************************************************************************
bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char puntoVirgola;
        Vector2i vertices;

        converter >>  id >> puntoVirgola >> marker >> puntoVirgola >> vertices(0) >> puntoVirgola >> vertices(1);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {
            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }

    file.close();

    return true;
}
// ***************************************************************************
bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh)
{
    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    listLines.pop_front();

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVertices.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        unsigned int numVertices;
        unsigned int numEdges;
        char puntoVirgola;

        converter >> id >> puntoVirgola >> marker >> puntoVirgola >> numVertices >> puntoVirgola;

        // in questo caso devo utilizzare dei vettori perchè la loro dimensione è
        // memorizzata nel file CSV
        vector<unsigned int> vertices(numVertices);

        for(unsigned int i = 0 ; i < numVertices ; i++)
        {
            converter >> vertices[i] >> puntoVirgola;
        }

        converter >> numEdges >> puntoVirgola;

        vector<unsigned int> edges(numEdges);

        for(unsigned int i = 0 ; i < numEdges ; i++)
        {
            converter >> edges[i] >> puntoVirgola;
        }

        mesh.Cell2DId.push_back(id);
        mesh.Cell2DVertices.push_back(vertices);
        mesh.Cell2DEdges.push_back(edges);

        if( marker != 0)
        {
            auto ret = mesh.Cell2DMarkers.insert({marker, {id}});
            if(!ret.second)
            {
                (ret.first)->second.push_back(id);
            }
        }



    }
    file.close();
    return true;
}

}
