/*********************************************************************
 * \file	Mat3.hpp
 * \brief	Defines a custom Matrix 3x3 library with getter and setter
			functions and a determinant function.
 *
 * \author	Wong Woon Li, woonli.wong, 2301308
 * \email	woonli.wong@digipen.edu
 * \date	20 September 2024

Copyright(C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 *********************************************************************/

// Matrix elements stored in memory using column-major order.
struct Mat3 {
	float a[9] = { 0 };

	/*!***********************************************************************
	\brief
		Getter/setter function to get matrix element at specified row and col.
	\param[in] unsigned int row
		Row of matrix to get/set.
	\param[in] unsigned int col
		Column of matrix to get/set.
	\return
		Reference to element of matrix at specified row and column.
	*************************************************************************/
	inline float& GetElement(unsigned int row, unsigned int col);

	/*!***********************************************************************
	\brief
		Getter function to get const matrix element at specified row and col.
	\param[in] unsigned int row
		Row of matrix to get/set.
	\param[in] unsigned int col
		Column of matrix to get/set.
	\return
		Const reference to element of matrix at specified row and column.
	*************************************************************************/
	inline const float& GetElement(unsigned int row, unsigned int col) const;

	/*!***********************************************************************
	\brief
		Returns the determinant of the Matrix.
	\return
		Determinant of the Matrix.
	*************************************************************************/
	inline float Determinant() const;
};

inline float& Mat3::GetElement(unsigned int row, unsigned int col)
{ 
	return a[col * 3 + row]; 
}

inline const float& Mat3::GetElement(unsigned int row, unsigned int col) const 
{ 
	return a[col * 3 + row]; 
}

inline float Mat3::Determinant() const {
	return GetElement(0, 0) * (GetElement(1, 1) * GetElement(2, 2) - GetElement(1, 2) * GetElement(2, 1))
		 - GetElement(0, 1) * (GetElement(1, 0) * GetElement(2, 2) - GetElement(1, 2) * GetElement(2, 0))
		 + GetElement(0, 2) * (GetElement(1, 0) * GetElement(2, 1) - GetElement(1, 1) * GetElement(2, 0));
}
