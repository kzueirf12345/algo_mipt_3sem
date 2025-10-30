#include <iostream>
#include <vector>

int main()
{
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int rowCount = 0;
    int colCount = 0;
    std::cin >> rowCount >> colCount;

    std::vector<std::vector<int>> matrix(rowCount, std::vector<int>(colCount));
    std::vector<int> rowSums(rowCount, 0);
    std::vector<int> colSums(colCount, 0);

    for (size_t i = 0; i < static_cast<size_t>(rowCount); ++i)
    {
        for (size_t j = 0; j < static_cast<size_t>(colCount); ++j)
        {
            std::cin >> matrix[i][j];
            rowSums[i] += matrix[i][j];
            colSums[j] += matrix[i][j];
        }
    }

    std::vector<int> flippedRows;
    std::vector<int> flippedCols;
    std::vector<bool> flipSequence;

    while (true)
    {
        bool modified = false;

        for (size_t i = 0; i < static_cast<size_t>(rowCount); ++i)
        {
            if (rowSums[i] < 0)
            {
                flippedRows.push_back(static_cast<int>(i));
                flipSequence.push_back(true);
                rowSums[i] = -rowSums[i];

                for (size_t j = 0; j < static_cast<size_t>(colCount); ++j)
                {
                    matrix[i][j] = -matrix[i][j];
                    colSums[j] += matrix[i][j] * 2;
                }
                modified = true;
            }
        }

        if (!modified)
        {
            for (size_t j = 0; j < static_cast<size_t>(colCount); ++j)
            {
                if (colSums[j] < 0)
                {
                    flippedCols.push_back(static_cast<int>(j));
                    flipSequence.push_back(false);
                    colSums[j] = -colSums[j];

                    for (size_t i = 0; i < static_cast<size_t>(rowCount); ++i)
                    {
                        matrix[i][j] = -matrix[i][j];
                        rowSums[i] += matrix[i][j] * 2;
                    }
                    modified = true;
                }
            }
        }

        if (!modified)
        {
            break;
        }
    }

    bool isPossible = true;

    for (size_t i = 0; i < static_cast<size_t>(rowCount); ++i)
    {
        if (rowSums[i] < 0)
        {
            isPossible = false;
        }
    }

    for (size_t j = 0; j < static_cast<size_t>(colCount); ++j)
    {
        if (colSums[j] < 0)
        {
            isPossible = false;
        }
    }

    if (isPossible)
    {
        size_t seqSize = flipSequence.size();
        size_t rowIndex = 0;
        size_t colIndex = 0;

        for (size_t i = 0; i < seqSize; ++i)
        {
            if (flipSequence[i])
            {
                std::cout << "l " << flippedRows[rowIndex++] << "\n";
            }
            else
            {
                std::cout << "c " << flippedCols[colIndex++] << "\n";
            }
        }
    }
    else
    {
        std::cout << "Impossible\n";
    }

    return 0;
}
