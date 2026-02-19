/*********************************************************************
 * \file	MathTest.hpp
 * \brief	Test various Math functionalities.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

#include "Mat4.hpp"
#include <iostream>
#include <cassert>

void TestDeterminant() {
    Mat4 mat;
    mat.SetTo(1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16);

    float det = mat.Determinant();
    std::cout << "Determinant: " << det << std::endl;

    // The determinant of the above matrix is 0 (it's singular).
    assert(det == 0.0f);  // Check if the determinant is correct
}

void TestDeterminantNonZero() {
    Mat4 mat;
    mat.SetTo(1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    float det = mat.Determinant();
    std::cout << "Determinant (identity matrix): " << det << std::endl;

    assert(det == 1.0f);  // Determinant of an identity matrix is 1
}

void TestTranspose() {
    Mat4 mat;
    mat.SetTo(1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16);

    Mat4 transposeMat = mat.Transpose();

    // Check that rows and columns are swapped
    assert(transposeMat.GetElement(0, 0) == mat.GetElement(0, 0));
    assert(transposeMat.GetElement(0, 1) == mat.GetElement(1, 0));
    assert(transposeMat.GetElement(1, 0) == mat.GetElement(0, 1));
    assert(transposeMat.GetElement(2, 3) == mat.GetElement(3, 2));

    std::cout << "Transpose test passed!" << std::endl;
}

void TestInverse() {
    Mat4 mat;
    mat.SetTo(4, 7, 2, 3,
        3, 6, 1, 4,
        2, 5, 9, 8,
        1, 2, 3, 4);

    Mat4 inverseMat = mat.Inverse();

    // Multiply the original matrix by its inverse
    Mat4 identityMat = mat * inverseMat;

    // Identity matrix check (approximate due to floating-point precision)
    Mat4 expectedIdentity;
    expectedIdentity.SetToIdentity();

    for (unsigned int i = 0; i < 4; ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
            assert(fabs(identityMat.GetElement(i, j) - expectedIdentity.GetElement(i, j)) < 1e-6);
        }
    }

    std::cout << "Inverse test passed!" << std::endl;
}

void TestIdentityMatrix() {
    Mat4 identityMat;
    identityMat.SetToIdentity();

    for (unsigned int i = 0; i < 4; ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
            if (i == j) {
                assert(identityMat.GetElement(i, j) == 1.0f);
            }
            else {
                assert(identityMat.GetElement(i, j) == 0.0f);
            }
        }
    }

    std::cout << "Identity matrix test passed!" << std::endl;
}

void TestMatrixMultiplication() {
    Mat4 mat1;
    mat1.SetTo(1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16);

    Mat4 mat2;
    mat2.SetTo(16, 15, 14, 13,
        12, 11, 10, 9,
        8, 7, 6, 5,
        4, 3, 2, 1);

    Mat4 result = mat1 * mat2;

    // Check specific elements of the resulting matrix
    assert(result.GetElement(0, 0) == 80);  // Manually computed value
    assert(result.GetElement(0, 1) == 70);
    assert(result.GetElement(1, 0) == 240);
    assert(result.GetElement(3, 3) == 386);

    std::cout << "Matrix multiplication test passed!" << std::endl;
}

void TestMatrixRotation() {
    Mat4 mRot = Mat4::BuildRotation(16.66f, Vec3{ 0.4816f, 0.2408f, 0.8426f });
    for (unsigned int i = 0; i < 16; ++i) {
        std::cout << mRot[i] << std::endl;
    }
}