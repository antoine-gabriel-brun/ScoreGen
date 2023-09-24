#pragma region head

	#pragma once

	using namespace System;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::Drawing;
	using namespace System::Drawing::Drawing2D;

	#include "Music.h"
	#include "MathForMusic.h"

	using namespace ScoreGenLib::Music;

#pragma endregion

namespace ScoreGenLib {
namespace Generators {

	// Objet abstrait capable de g�n�rer un Score et des informations associ�es (classe de base � d�river)
	public ref class Generator abstract {

		// A impl�menter
		public:
			virtual String^        name       () = 0;
			virtual String^        description() = 0;
			virtual ScoreAndInfos^ generate   () = 0;

	};

}
}
