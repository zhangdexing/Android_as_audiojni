

#include "Utilities/matrix.h"

/*******************************************************************************
 * matrixCreate     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      nRows           Number of rows                                 *
 *              nCols           Number of columns                              *
 *              nFrames         Number of frames                               *
 *                                                                             *
 * Outputs:     objMatrix       The new matrix object                          *
 *                                                                             *
 * Description: This function creates a 2D matrix for each of the nFrames. All *
 *              values are set to zero. Memory is dynamically allocated.       *
 *                                                                             *
 ******************************************************************************/

struct objMatrix* matrixCreate(unsigned int nRows, unsigned int nCols, unsigned int nFrames)
{

    struct objMatrix* newMatrix;

    // Create the object
    newMatrix = (struct objMatrix*) malloc(sizeof(struct objMatrix));

    // Initialize its fields
    matrixInit(newMatrix,nRows,nCols,nFrames);

    return newMatrix;

}

/*******************************************************************************
 * matrixDelete     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function deletes the matrix and frees the memory.         *
 *                                                                             *
 ******************************************************************************/

void matrixDelete(struct objMatrix* matrix)
{

    // Terminate its fields
    matrixTerminate(matrix);

    // Delete the object
    free((void*) matrix);

}

/*******************************************************************************
 * matrixInit     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      nRows           Number of rows                                 *
 *              nCols           Number of columns                              *
 *              nFrames         Number of frames                               *
 *                                                                             *
 * Outputs:     matrix          The matrix object                              *
 *                                                                             *
 * Description: Memory is dynamically allocated.                               *
 *                                                                             *
 ******************************************************************************/

void matrixInit(struct objMatrix* matrix, unsigned int nRows, unsigned int nCols, unsigned int nFrames)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;



    // Set dimensions
    matrix->nRows = nRows;
    matrix->nCols = nCols;
    matrix->nFrames = nFrames;

    if ((matrix->nRows > 0) && (matrix->nCols > 0) && (matrix->nFrames > 0))
    {

        // Then create a dynamic array that will contain each element
        matrix->valueReal = (float***) newTable3D(nRows,nCols,nFrames,sizeof(float));
        matrix->valueImag = (float***) newTable3D(nRows,nCols,nFrames,sizeof(float));

        // Fill the matrix with zeros everywhere
        for (indexRow = 0; indexRow < matrix->nRows; indexRow++)
        {

            for (indexCol = 0; indexCol < matrix->nCols; indexCol++)
            {


                for (k = 0; k < matrix->nFrames; k++)
                {

                    matrix->valueReal[indexRow][indexCol][k] = 0.0;
                    matrix->valueImag[indexRow][indexCol][k] = 0.0;

                }


            }

        }

    }

}

/*******************************************************************************
 * matrixTerminate     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function frees the memory.                                *
 *                                                                             *
 ******************************************************************************/

void matrixTerminate(struct objMatrix* matrix)
{

    // Delete old arrays if no dimension was null
    if ((matrix->nRows > 0) && (matrix->nCols > 0) && (matrix->nFrames > 0))
    {
        deleteTable3D((void***) matrix->valueReal);
        deleteTable3D((void***) matrix->valueImag);
    }

}

/*******************************************************************************
 * matrixCopy     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrixSource    The matrix object that is copied               *
 *                                                                             *
 * Outputs:     matrixDest      The matrix object that receives the copy       *
 *                                                                             *
 * Description: This function copies the dimensions and the content of a       *
 *              matrix inside another matrix                                   *
 *                                                                             *
 ******************************************************************************/

void matrixCopy(struct objMatrix* matrixSource, struct objMatrix* matrixDest)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;


    if ((matrixSource->nCols == matrixDest->nCols) &&
        (matrixSource->nRows == matrixDest->nRows) &&
        (matrixSource->nFrames == matrixDest->nFrames))
        {

        // Copy the content
        for (indexRow = 0; indexRow < matrixSource->nRows; indexRow++)
        {
            for (indexCol = 0; indexCol < matrixSource->nCols; indexCol++)
            {

                for (k = 0; k < matrixSource->nFrames; k++)
                {
                    matrixSetReal(matrixDest,indexRow,indexCol,k,matrixGetReal(matrixSource,indexRow,indexCol,k));
                    matrixSetImag(matrixDest,indexRow,indexCol,k,matrixGetImag(matrixSource,indexRow,indexCol,k));
                }

            }
        }
    }
}

/*******************************************************************************
 * matrixGetReal     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              indexRow        The index of the row                           *
 *              indexCol        The index of the column                        *
 *              k               The index of the frame                         *
 *                                                                             *
 * Outputs:     (returned value)                                               *
 *                                                                             *
 * Description: This function returns the real value of the kth 2D matrix at   *
 *              a given row and column                                         *
 *                                                                             *
 ******************************************************************************/

float matrixGetReal(struct objMatrix* matrix, unsigned int indexRow, unsigned int indexCol, unsigned int k)
{
    return matrix->valueReal[indexRow][indexCol][k];
}

/*******************************************************************************
 * matrixGetImag     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              indexRow        The index of the row                           *
 *              indexCol        The index of the column                        *
 *              k               The index of the frame                         *
 *                                                                             *
 * Outputs:     (returned value)                                               *
 *                                                                             *
 * Description: This function returns the imaginary value of the kth 2D matrix *
 *              at a given row and column                                      *
 *                                                                             *
 ******************************************************************************/

float matrixGetImag(struct objMatrix* matrix, unsigned int indexRow, unsigned int indexCol, unsigned int k)
{
    return matrix->valueImag[indexRow][indexCol][k];
}

/*******************************************************************************
 * matrixSetReal     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              indexRow        The index of the row                           *
 *              indexCol        The index of the column                        *
 *              k               The index of the frame                         *
 *              value           The value to load                              *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function sets the real value of the kth 2D matrix at      *
 *              a given row and column to the given value                      *
 *                                                                             *
 ******************************************************************************/

void matrixSetReal(struct objMatrix* matrix, unsigned int indexRow, unsigned int indexCol, unsigned int k, float value)
{
    matrix->valueReal[indexRow][indexCol][k] = value;
}

/*******************************************************************************
 * matrixSetImag     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              indexRow        The index of the row                           *
 *              indexCol        The index of the column                        *
 *              k               The index of the frame                         *
 *              value           The value to load                              *
 *                                                                             *
 * Outputs:     (none)                                                         *
 *                                                                             *
 * Description: This function sets the imaginary value of the kth 2D matrix at *
 *              a given row and column to the given value                      *
 *                                                                             *
 ******************************************************************************/

void matrixSetImag(struct objMatrix* matrix, unsigned int indexRow, unsigned int indexCol, unsigned int k, float value)
{
    matrix->valueImag[indexRow][indexCol][k] = value;
}

/*******************************************************************************
 * matrixInsertRow     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              newRowIndex     The index of the row                           *
 *                                                                             *
 * Outputs:     matrix          The matrix object                              *
 *                                                                             *
 * Description: This function inserts a new row at the specified index.        *
 *                                                                             *
 ******************************************************************************/

void matrixInsertRow(struct objMatrix* matrix, unsigned int newRowIndex)
{

    struct objMatrix* oldMatrix;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;



    // Copy old matrix
    oldMatrix = matrixCreate(matrix->nRows, matrix->nCols, matrix->nFrames);
    matrixCopy(matrix,oldMatrix);

    // Resize the matrix
    matrixResize(matrix,matrix->nRows+1,matrix->nCols,matrix->nFrames);

    // Copy the old content
    for (indexRow = 0; indexRow < newRowIndex; indexRow++)
    {
        for (indexCol = 0; indexCol < oldMatrix->nCols; indexCol++)
        {

            for (k = 0; k < oldMatrix->nFrames; k++)
            {
                matrixSetReal(matrix, indexRow, indexCol, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                matrixSetImag(matrix, indexRow, indexCol, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
            }

        }
    }
    for (indexRow = newRowIndex; indexRow < oldMatrix->nRows; indexRow++)
    {
        for (indexCol = 0; indexCol < oldMatrix->nCols; indexCol++)
        {


            for (k = 0; k < oldMatrix->nFrames; k++)
            {
                matrixSetReal(matrix, indexRow+1, indexCol, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                matrixSetImag(matrix, indexRow+1, indexCol, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
            }

        }
    }

    matrixDelete(oldMatrix);

}

/*******************************************************************************
 * matrixInsertCol     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              newColIndex     The index of the col                           *
 *                                                                             *
 * Outputs:     matrix          The matrix object                              *
 *                                                                             *
 * Description: This function inserts a new column at the specified index.     *
 *                                                                             *
 ******************************************************************************/

void matrixInsertCol(struct objMatrix* matrix, unsigned int newColIndex)
{

    struct objMatrix* oldMatrix;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;


    // Copy old matrix
    oldMatrix = matrixCreate(0, 0, 0);
    matrixCopy(matrix,oldMatrix);

    // Resize the matrix
    matrixResize(matrix,matrix->nRows,matrix->nCols+1,matrix->nFrames);

    // Copy the old content
    for (indexCol = 0; indexCol < newColIndex; indexCol++)
    {
        for (indexRow = 0; indexRow < oldMatrix->nRows; indexRow++)
        {

            for (k = 0; k < oldMatrix->nFrames; k++)
            {
                matrixSetReal(matrix, indexRow, indexCol, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                matrixSetImag(matrix, indexRow, indexCol, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
            }
        }
    }
    for (indexCol = newColIndex; indexCol < oldMatrix->nCols; indexCol++)
    {
        for (indexRow = 0; indexRow < oldMatrix->nRows; indexRow++)
        {

            for (k = 0; k < oldMatrix->nFrames; k++)
            {
                matrixSetReal(matrix, indexRow, indexCol+1, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                matrixSetImag(matrix, indexRow, indexCol+1, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
            }
        }
    }

    matrixDelete(oldMatrix);

}

/*******************************************************************************
 * matrixDeleteRow     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              deleteRowIndex  The index of the row                           *
 *                                                                             *
 * Outputs:     matrix          The matrix object                              *
 *                                                                             *
 * Description: This function deletes the row at the specified index.          *
 *                                                                             *
 ******************************************************************************/

void matrixDeleteRow(struct objMatrix* matrix, unsigned int deleteRowIndex)
{

    struct objMatrix* oldMatrix = NULL;

    unsigned int indexRow = 0;
    unsigned int indexCol = 0;
    unsigned int k = 0;

    if (matrix->nRows > 0)
    {

        // Copy old matrix
        oldMatrix = matrixCreate(matrix->nRows, matrix->nCols, matrix->nFrames);
        matrixCopy(matrix,oldMatrix);

        // Resize the matrix
        matrixResize(matrix,matrix->nRows-1,matrix->nCols,matrix->nFrames);

        // Copy the old content
        for (indexRow = 0; indexRow < deleteRowIndex; indexRow++)
        {
            for (indexCol = 0; indexCol < oldMatrix->nCols; indexCol++)
            {

                for (k = 0; k < oldMatrix->nFrames; k++)
                {
                    matrixSetReal(matrix, indexRow, indexCol, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                    matrixSetImag(matrix, indexRow, indexCol, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
                }

            }
        }
        for (indexRow = (deleteRowIndex+1); indexRow < oldMatrix->nRows; indexRow++)
        {
            for (indexCol = 0; indexCol < oldMatrix->nCols; indexCol++)
            {

                for (k = 0; k < oldMatrix->nFrames; k++)
                {
                    matrixSetReal(matrix, indexRow-1, indexCol, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                    matrixSetImag(matrix, indexRow-1, indexCol, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
                }

            }
        }

    }

    matrixDelete(oldMatrix);

}

/*******************************************************************************
 * matrixDeleteCol     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              deleteColIndex  The index of the col                           *
 *                                                                             *
 * Outputs:     matrix          The matrix object                              *
 *                                                                             *
 * Description: This function deletes the column at the specified index.       *
 *                                                                             *
 ******************************************************************************/

void matrixDeleteCol(struct objMatrix* matrix, unsigned int deleteColIndex)
{

    struct objMatrix* oldMatrix = NULL;

    unsigned int indexRow = 0;
    unsigned int indexCol = 0;
    unsigned int k = 0;

    if (matrix->nCols > 0)
    {

        // Copy old matrix
        oldMatrix = matrixCreate(matrix->nRows, matrix->nCols, matrix->nFrames);
        matrixCopy(matrix,oldMatrix);

        // Resize the matrix
        matrixResize(matrix,matrix->nRows,matrix->nCols-1,matrix->nFrames);

        // Copy the old content
        for (indexCol = 0; indexCol < deleteColIndex; indexCol++)
        {
            for (indexRow = 0; indexRow < oldMatrix->nRows; indexRow++)
            {

                for (k = 0; k < oldMatrix->nFrames; k++)
                {
                    matrixSetReal(matrix, indexRow, indexCol, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                    matrixSetImag(matrix, indexRow, indexCol, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
                }

            }
        }
        for (indexCol = (deleteColIndex+1); indexCol < oldMatrix->nCols; indexCol++)
        {
            for (indexRow = 0; indexRow < oldMatrix->nRows; indexRow++)
            {

                for (k = 0; k < oldMatrix->nFrames; k++)
                {
                    matrixSetReal(matrix, indexRow, indexCol-1, k, matrixGetReal(oldMatrix, indexRow, indexCol, k));
                    matrixSetImag(matrix, indexRow, indexCol-1, k, matrixGetImag(oldMatrix, indexRow, indexCol, k));
                }

            }
        }

    }

    matrixDelete(oldMatrix);
}

/*******************************************************************************
 * matrixResize     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *              nRows           Number of rows                                 *
 *              nCols           Number of columns                              *
 *              nFrames         Number of frames                               *
 *                                                                             *
 * Outputs:     matrix          The matrix object                              *
 *                                                                             *
 * Description: This function resizes the matrix according to the new          *
 *              dimensions provided. All elements are set to zero.             *
 *                                                                             *
 ******************************************************************************/

void matrixResize(struct objMatrix* matrix, unsigned int nRows, unsigned int nCols, unsigned int nFrames)
{

    // Terminate
    matrixTerminate(matrix);

    // And initialize again with new parameters
    matrixInit(matrix,nRows,nCols,nFrames);

}

/*******************************************************************************
 * matrixMultMatrix                                                            *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrixA         The matrix object which is the first operand   *
 *              matrixB         The matrix object which is the second operand  *
 *                                                                             *
 * Outputs:     matrixResult    The matrix object which holds the result       *
 *                                                                             *
 * Description: This function multiplies matrixA by matrixB for all frames.    *
 *              The result is stored in matrixResult.                          *
 *                                                                             *
 *      R(0) = A(0) * B(0)                                                     *
 *      R(1) = A(1) * B(1)                                                     *
 *      ...        ...                                                         *
 *      R(k) = A(k) * B(k)                                                     *
 *      ...        ...                                                         *
 *      R(K) = A(K) * B(K)                                                     *
 *                                                                             *
 ******************************************************************************/

void matrixMultMatrix(struct objMatrix* matrixA, struct objMatrix* matrixB, struct objMatrix* matrixResult)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int indexLine;
    unsigned int k;

    unsigned int newNRows;
    unsigned int newNCols;
    unsigned int newNLines;
    unsigned int newNFrames;

    float elementAReal;
    float elementAImag;
    float elementBReal;
    float elementBImag;


    // Check if dimensions match
    if ((matrixA->nCols == matrixB->nRows) && (matrixA->nFrames == matrixB->nFrames))
    {

        newNRows = matrixA->nRows;
        newNCols = matrixB->nCols;
        newNLines = matrixA->nCols;
        newNFrames = matrixA->nFrames;

        // Resize the matrix that will contain the result if needed
        if ((matrixResult->nRows != newNRows) || (matrixResult->nCols != newNCols) || (matrixResult->nFrames != newNFrames))
        {
            matrixResize(matrixResult,newNRows,newNCols,newNFrames);
        }

        // Loop for each element
        for (indexRow = 0; indexRow < newNRows; indexRow++)
        {
            for (indexCol = 0; indexCol < newNCols; indexCol++)
            {

                for (k = 0; k < newNFrames; k++)
                {                 

                    matrixResult->valueReal[indexRow][indexCol][k] = 0.0;
                    matrixResult->valueImag[indexRow][indexCol][k] = 0.0;
                }

                for (indexLine = 0; indexLine < newNLines; indexLine++)
                {

                    for (k = 0; k < newNFrames; k++)
                    {

                        elementAReal = matrixGetReal(matrixA,indexRow,indexLine,k);
                        elementAImag = matrixGetImag(matrixA,indexRow,indexLine,k);
                        elementBReal = matrixGetReal(matrixB,indexLine,indexCol,k);
                        elementBImag = matrixGetImag(matrixB,indexLine,indexCol,k);

                        matrixResult->valueReal[indexRow][indexCol][k] += elementAReal * elementBReal - elementAImag * elementBImag;
                        matrixResult->valueImag[indexRow][indexCol][k] += elementAReal * elementBImag + elementAImag * elementBReal;

                    }

                }

            }
        }

    }
    else
    {
        // Create an empty matrix
        matrixResult = matrixCreate(0,0,0);
    }

}

/*******************************************************************************
 * matrixMultScalar                                                            *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrixSource    The matrix object which is the first operand   *
 *              scalar          The scalar value which is the second operand   *
 *                                                                             *
 * Outputs:     matrixResult    The matrix object which holds the result       *
 *                                                                             *
 * Description: This function multiplies matrixA by the scalar for all frames. *
 *              The result is stored in matrixResult.                          *
 *                                                                             *
 *      R(0) = scalar * A(0)                                                   *
 *      R(1) = scalar * A(1)                                                   *
 *      ...        ...                                                         *
 *      R(k) = scalar * A(k)                                                   *
 *      ...        ...                                                         *
 *      R(K) = scalar * A(K)                                                   *
 *                                                                             *
 ******************************************************************************/

void matrixMultScalar(struct objMatrix* matrixSource, float scalar, struct objMatrix* matrixResult)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    unsigned int newNRows;
    unsigned int newNCols;
    unsigned int newNFrames;


    newNRows = matrixSource->nRows;
    newNCols = matrixSource->nCols;
    newNFrames = matrixSource->nFrames;

    // Resize the matrix that will contain the result if needed
    if ((matrixResult->nRows != newNRows) || (matrixResult->nCols != newNCols) || (matrixResult->nFrames != newNFrames))
    {
        matrixResize(matrixResult,newNRows,newNCols,newNFrames);
    }


    // Loop for each element
    for (indexRow = 0; indexRow < newNRows; indexRow++)
    {
        for (indexCol = 0; indexCol < newNCols; indexCol++)
        {

            for (k = 0; k < newNFrames; k++)
            {

                matrixResult->valueReal[indexRow][indexCol][k] = matrixSource->valueReal[indexRow][indexCol][k] * scalar;
                matrixResult->valueImag[indexRow][indexCol][k] = matrixSource->valueImag[indexRow][indexCol][k] * scalar;

            }

        }
    }

}

/*******************************************************************************
 * matrixRemoveIdentity                                                        *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *                                                                             *
 * Outputs:     matrixResult    The matrix object which holds the result       *
 *                                                                             *
 * Description: This function removes the identity matrix for each frame.      *
 *              The result is stored in matrixResult.                          *
 *                                                                             *
 *      R(0) = A(0) - I                                                        *
 *      R(1) = A(1) - I                                                        *
 *      ...        ...                                                         *
 *      R(k) = A(k) - I                                                        *
 *      ...        ...                                                         *
 *      R(K) = A(K) - I                                                        *
 *                                                                             *
 ******************************************************************************/

void matrixRemoveIdentity(struct objMatrix* matrix)
{

    unsigned int indexRowCol;
    unsigned int k;

    unsigned int nRowsCols;


    // Make sure the matrix is square
    if (matrix->nRows == matrix->nCols)
    {

        nRowsCols = matrix->nRows;

        for (indexRowCol = 0; indexRowCol < nRowsCols; indexRowCol++)
        {


            for (k = 0; k < matrix->nFrames; k++)
            {
                matrix->valueReal[indexRowCol][indexRowCol][k] -= 1.0f;
            }

        }

    }

}

/*******************************************************************************
 * matrixRemoveIdentity                                                        *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object                              *
 *                                                                             *
 * Outputs:     matrixResult    The matrix object which holds the result       *
 *                                                                             *
 * Description: This function sets the diagonal term to zero for each frame.   *
 *              The result is stored in matrixResult.                          *
 *                                                                             *
 *      R(0) = A(0) & (!I)                                                     *
 *      R(1) = A(1) & (!I)                                                     *
 *      ...        ...                                                         *
 *      R(k) = A(k) & (!I)                                                     *
 *      ...        ...                                                         *
 *      R(K) = A(K) & (!I)                                                     *
 *                                                                             *
 ******************************************************************************/

void matrixRemoveDiagonal(struct objMatrix* matrix)
{

    unsigned int indexRowCol;
    unsigned int k;

    unsigned int nRowsCols;


    // Make sure the matrix is square
    if (matrix->nRows == matrix->nCols)
    {

        nRowsCols = matrix->nRows;


        for (indexRowCol = 0; indexRowCol < nRowsCols; indexRowCol++)
        {

            for (k = 0; k < matrix->nFrames; k++)
            {
                matrix->valueReal[indexRowCol][indexRowCol][k] = 0.0f;
                matrix->valueImag[indexRowCol][indexRowCol][k] = 0.0f;
            }

        }

    }

}

/*******************************************************************************
 * matrixHermitian     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrixSource    The matrix object used                         *
  *                                                                            *
 * Outputs:     matrixDest      The matrix object that holds the result        *
 *                                                                             *
 * Description: This function computes the hermitian matrix.                   *
 *                                                                             *
 ******************************************************************************/

void matrixHermitian(struct objMatrix* matrixSource, struct objMatrix* matrixDest)
{

    unsigned int newNRows;
    unsigned int newNCols;
    unsigned int newNFrames;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;


    newNRows = matrixSource->nCols;
    newNCols = matrixSource->nRows;
    newNFrames = matrixSource->nFrames;

    // Resize the matrix that will contain the result if needed
    if ((matrixDest->nRows != newNRows) || (matrixDest->nCols != newNCols) || (matrixDest->nFrames != newNFrames))
    {
        matrixResize(matrixDest,newNRows,newNCols,newNFrames);
    }


    // Move content
    for (indexRow = 0; indexRow < matrixSource->nRows; indexRow++)
    {
        for (indexCol = 0; indexCol < matrixSource->nCols; indexCol++)
        {


            for (k = 0; k < matrixSource->nFrames; k++)
            {
                matrixSetReal(matrixDest,indexCol,indexRow,k,matrixGetReal(matrixSource,indexRow,indexCol,k));
                matrixSetImag(matrixDest,indexCol,indexRow,k,-1.0f * matrixGetImag(matrixSource,indexRow,indexCol,k));
            }


        }
    }

}

void matrixMultMatrixPerElement(struct objMatrix* matrixA, struct objMatrix* matrixB, struct objMatrix* matrixResult)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    float elementAReal;
    float elementAImag;
    float elementBReal;
    float elementBImag;
    float resultReal;
    float resultImag;


    if ((matrixA->nCols == matrixB->nCols) && (matrixA->nRows == matrixB->nRows))
    {

        nRows = matrixA->nRows;
        nCols = matrixA->nCols;
        nFrames = matrixA->nFrames;

        if ((matrixResult->nRows != nRows) || (matrixResult->nCols != nCols) || (matrixResult->nFrames != nFrames))
        {
            matrixResize(matrixResult,nRows,nCols,nFrames);
        }

        for (indexRow = 0; indexRow < nRows; indexRow++)
        {

            for (indexCol = 0; indexCol < nCols; indexCol++)
            {


                for (k = 0; k < nFrames; k++)
                {

                    elementAReal = matrixGetReal(matrixA,indexRow,indexCol,k);
                    elementAImag = matrixGetImag(matrixA,indexRow,indexCol,k);
                    elementBReal = matrixGetReal(matrixB,indexRow,indexCol,k);
                    elementBImag = matrixGetImag(matrixB,indexRow,indexCol,k);

                    // Re{A*B} = Re{A}*Re{B} - Im{A}*Im{B}
                    resultReal = elementAReal * elementBReal - elementAImag * elementBImag;

                    // Im{A*B} = Re{A}*Im{B} + Im{A}*Re{B}
                    resultImag = elementAReal * elementBImag + elementAImag * elementBReal;

                    matrixSetReal(matrixResult,indexRow,indexCol,k,resultReal);
                    matrixSetImag(matrixResult,indexRow,indexCol,k,resultImag);

                }

            }

        }

    }

}

void matrixDividePerElement(struct objMatrix* matrixA, struct objMatrix* matrixB, struct objMatrix* matrixResult)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    float elementAReal;
    float elementAImag;
    float elementBReal;
    float elementBImag;
    float resultReal;
    float resultImag;

    if ((matrixA->nCols == matrixB->nCols) && (matrixA->nRows == matrixB->nRows))
    {

        nRows = matrixA->nRows;
        nCols = matrixA->nCols;
        nFrames = matrixA->nFrames;

        if ((matrixResult->nRows != nRows) || (matrixResult->nCols != nCols) || (matrixResult->nFrames != nFrames))
        {
            matrixResize(matrixResult,nRows,nCols,nFrames);
        }

        for (indexRow = 0; indexRow < nRows; indexRow++)
        {

            for (indexCol = 0; indexCol < nCols; indexCol++)
            {

                for (k = 0; k < nFrames; k++)
                {

                    elementAReal = matrixGetReal(matrixA,indexRow,indexCol,k);
                    elementAImag = matrixGetImag(matrixA,indexRow,indexCol,k);
                    elementBReal = matrixGetReal(matrixB,indexRow,indexCol,k);
                    elementBImag = matrixGetImag(matrixB,indexRow,indexCol,k);

                    // Re{A*B} = (Re{A}*Re{B} + Im{A}*Im{B})/(Re{B}*Re{B} + Im{B}*Im{B})
                    resultReal = (elementAReal * elementBReal + elementAImag * elementBImag) /
                                 (elementBReal * elementBReal + elementBImag * elementBImag);

                    // Im{A*B} = (Im{A}*Re{B} - Re{A}*Im{B})/(Re{B}*Re{B} + Im{B}*Im{B})
                    resultImag = (elementAImag * elementBReal - elementAReal * elementBImag) /
                                 (elementBReal * elementBReal + elementBImag * elementBImag);

                    matrixSetReal(matrixResult,indexRow,indexCol,k,resultReal);
                    matrixSetImag(matrixResult,indexRow,indexCol,k,resultImag);

                }
            }

        }

    }

}

void matrixInvRealPerElement(struct objMatrix* matrixSource, struct objMatrix* matrixResult)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    float element;
    float result;

    nRows = matrixSource->nRows;
    nCols = matrixSource->nCols;
    nFrames = matrixSource->nFrames;

    if ((matrixResult->nRows != nRows) || (matrixResult->nCols != nCols) || (matrixResult->nFrames != nFrames))
    {
        matrixResize(matrixResult,nRows,nCols,nFrames);
    }

    for (indexRow = 0; indexRow < nRows; indexRow++)
    {

        for (indexCol = 0; indexCol < nCols; indexCol++)
        {


            for (k = 0; k < nFrames; k++)
            {

                element = matrixGetReal(matrixSource,indexRow,indexCol,k);

                result = (float) (1.0/(element));

                matrixSetReal(matrixResult,indexRow,indexCol,k,result);

            }

        }

    }


}

void matrixMultScalarPerFrame(struct objMatrix* matrixSource, struct objMatrix* matrixFrames, struct objMatrix* matrixResult)
{

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    float elementReal;
    float elementImag;
    float scalar;
    float resultReal;
    float resultImag;

    nRows = matrixSource->nRows;
    nCols = matrixSource->nCols;
    nFrames = matrixSource->nFrames;   

    if ((matrixResult->nRows != nRows) || (matrixResult->nCols != nCols) || (matrixResult->nFrames != nFrames))
    {
        matrixResize(matrixResult,nRows,nCols,nFrames);
    }

    for (indexRow = 0; indexRow < nRows; indexRow++)
    {

        for (indexCol = 0; indexCol < nCols; indexCol++)
        {


            for (k = 0; k < nFrames; k++)
            {

                scalar = matrixGetReal(matrixFrames,0,0,k);

                elementReal = matrixGetReal(matrixSource,indexRow,indexCol,k);
                elementImag = matrixGetImag(matrixSource,indexRow,indexCol,k);

                resultReal = scalar * elementReal;
                resultImag = scalar * elementImag;

                matrixSetReal(matrixResult,indexRow,indexCol,k,resultReal);
                matrixSetImag(matrixResult,indexRow,indexCol,k,resultImag);

            }

        }

    }
}

void matrixAddMatrix(struct objMatrix* matrixA, struct objMatrix* matrixB, struct objMatrix* matrixResult)
{

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    float elementAReal;
    float elementAImag;
    float elementBReal;
    float elementBImag;
    float resultReal;
    float resultImag;


    if ((matrixA->nRows == matrixB->nRows) && (matrixA->nCols == matrixB->nCols) && (matrixA->nFrames == matrixB->nFrames))
    {

        nRows = matrixA->nRows;
        nCols = matrixA->nCols;
        nFrames = matrixA->nFrames;

        if ((matrixA->nRows != nRows) || (matrixA->nCols != nCols) || (matrixA->nFrames != nFrames))
        {
            matrixResize(matrixResult,nRows,nCols,nFrames);
        }

        for (indexRow = 0; indexRow < nRows; indexRow++)
        {
            for (indexCol = 0; indexCol < nCols; indexCol++)
            {


                for (k = 0; k < nFrames; k++)
                {
                    elementAReal = matrixGetReal(matrixA,indexRow,indexCol,k);
                    elementAImag = matrixGetImag(matrixA,indexRow,indexCol,k);
                    elementBReal = matrixGetReal(matrixB,indexRow,indexCol,k);
                    elementBImag = matrixGetImag(matrixB,indexRow,indexCol,k);

                    // Re{A+B} = Re{A} + Re{B}
                    resultReal = elementAReal + elementBReal;
                    // Im{A+B} = Im{A} + Im{B}
                    resultImag = elementAImag + elementBImag;

                    matrixSetReal(matrixResult,indexRow,indexCol,k,resultReal);
                    matrixSetImag(matrixResult,indexRow,indexCol,k,resultImag);
                }

            }
        }

    }


}

void matrixMakeNonZero(struct objMatrix* matrix)
{

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    float minValue = (float) (1E-10);

    nRows = matrix->nRows;
    nCols = matrix->nCols;
    nFrames = matrix->nFrames;

    for (indexRow = 0; indexRow < nRows; indexRow++)
    {
        for (indexCol = 0; indexCol < nCols; indexCol++)
        {

            for (k = 0; k < nFrames; k++)
            {
                if ((matrixGetReal(matrix,indexRow,indexCol,k) == 0) && (matrixGetImag(matrix,indexRow,indexCol,k)==0))
                {
                    matrixSetReal(matrix,indexRow,indexCol,k,minValue);
                }
            }

        }
    }
}

/*******************************************************************************
 * matrixPrint     		                                               *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix          The matrix object used                         *
  *                                                                            *
 * Outputs:     (console)                                                      *
 *                                                                             *
 * Description: This function prints the content of a matrix.                  *
 *                                                                             *
 ******************************************************************************/

void matrixPrint(struct objMatrix* matrix)
{

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    if ((matrix->nRows == 0) || (matrix->nCols == 0) || (matrix->nFrames == 0))
    {
        printf("(Empty)\n");
    }
    else
    {
        for (k = 0; k < matrix->nFrames; k++)
        {

            printf("\nFrame: %i\n\n",k);

            for (indexRow = 0; indexRow < matrix->nRows; indexRow++)
            {

                for (indexCol = 0; indexCol < matrix->nCols; indexCol++)
                {

                    printf("%1.2E + j %1.2E     ",matrixGetReal(matrix,indexRow,indexCol,k),matrixGetImag(matrix,indexRow,indexCol,k));

                }

                printf("\n");
            }

        }

        printf("\n");

    }
}

void matrixPrintOneFrame(struct objMatrix* matrix, unsigned int k)
{

    unsigned int indexRow = 0;
    unsigned int indexCol = 0;

    if ((matrix->nRows == 0) || (matrix->nCols == 0) || (matrix->nFrames == 0))
    {
        printf("(Empty)\n");
    }
    else
    {

        printf("\nFrame: %i\n\n",k);

        for (indexRow = 0; indexRow < matrix->nRows; indexRow++)
        {

            for (indexCol = 0; indexCol < matrix->nCols; indexCol++)
            {

                printf("%1.2E + j %1.2E     ",matrixGetReal(matrix,indexRow,indexCol,k),matrixGetImag(matrix,indexRow,indexCol,k));

            }

            printf("\n");
        }

        printf("\n");
    }
}


/*******************************************************************************
 * matrixAddRowMatrix 		                                                   *
 * --------------------------------------------------------------------------- *
 *                                                                             *
 * Inputs:      matrix A         The matrix object used                        *
  *                                                                            *
 * Outputs:     matrix B                                                       *
 *                                                                             *
 * Description: 
 *   B(1) = A(1,1)+ A(2,1)+...+A(v,1)
 *   B(2) = A(1,2)+ A(2,2)+...+A(v,2)
 *   B(3) = A(1,3)+ A(2,3)+...+A(v,3)                                         *

 ******************************************************************************/


void  matrixAddRowMatrix(struct objMatrix* matrixA, struct objMatrix* matrixResult)
{

    unsigned int nRows;
    unsigned int nCols;
    unsigned int nFrames;

    unsigned int indexRow;
    unsigned int indexCol;
    unsigned int k;

    float elementAReal;
    float elementAImag;

    float resultReal = 0;
    float resultImag = 0;

    nRows = matrixA->nRows;
    nCols = matrixA->nCols;
    nFrames = matrixA->nFrames;


    for (k = 0; k < nFrames; k++)

    {

        for (indexCol = 0; indexCol < nCols; indexCol++)
        {

            for (indexRow = 0; indexRow < nRows; indexRow++)
            {

                elementAReal = matrixGetReal(matrixA, indexRow, indexCol, k);
                elementAImag = matrixGetImag(matrixA, indexRow, indexCol, k);

                resultReal = resultReal + elementAReal;
                resultImag = resultImag + elementAImag;
 
            }

            resultReal = resultReal / nRows;
            resultImag = resultImag / nRows;

            matrixSetReal(matrixResult, 0, indexCol, k, resultReal);
            matrixSetImag(matrixResult, 0, indexCol, k, resultImag);

            resultReal = 0;
            resultImag = 0;

        }

    }


}
