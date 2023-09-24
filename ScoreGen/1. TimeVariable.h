#pragma region head

	#pragma once

	using namespace System;
	using namespace ScoreGenLib::Music;

#pragma endregion

namespace ScoreGenLib {
namespace MathForMusic {
	
	// Valeur variant au cours du temps (classe de base)
	public ref class TimeVariable abstract {
				
		// Fonctions publiques : valeur
		public:
			double at(double time01) {
				
				if      (time01 <= 0) return at_internal(0       );
				else if (time01 >= 1) return at_internal(1       );
				else                    return at_internal(time01);
	
			}
			double at(double pos, int totalLength) {
	
				if      (pos <= 0              ) return at_internal(0                      );
				else if (pos >= totalLength - 1) return at_internal(1                      );
				else                             return at_internal(pos / (totalLength - 1));
	
			}
		
		// Fonctions publiques : affichage
		public:
			Bitmap^ image(int w, int h) {
				
				if (w < 10 || h < 10) throw gcnew Exception("The dimensions should be at least 10 * 10.");
	
				// Créer l'image
				Bitmap^ ans = gcnew Bitmap(w, h);
				Graphics^ g = Graphics::FromImage(ans);
				g->Clear(Color::White);
				g->SmoothingMode = SmoothingMode::HighQuality;
	
				// Calculer les valeurs pour chaque position horizontale
				array<double, 1>^ values = gcnew array<double, 1>(w);
				double minVal = Double::MaxValue;
				double maxVal = Double::MinValue;
				{
					for (int x = 0; x < w; x++) {
						double val = at(x, w);
						values[x] = val;
						minVal = Math::Min(minVal, val);
						maxVal = Math::Max(maxVal, val);
					}
				}
	
				// Décider des bornes d'affichage verticales
				{
					if (minVal < 0 && maxVal < 0) {
						maxVal = 0;
					} else if (minVal > 0 && maxVal > 0) {
						minVal = 0;
					} else if (minVal == 0 && maxVal == 0) {
						minVal = -1;
						maxVal =  1;
					}
					double diffMid = 0.5 * (maxVal - minVal);
					double mid = 0.5 * (minVal + maxVal);
					minVal = mid - 1.1 * diffMid;
					maxVal = mid + 1.1 * diffMid;
				}
	
				// Placer tous les points
				array<PointF, 1>^ points = gcnew array<PointF, 1>(w);
				for (int x = 0; x < w; x++) {
					points[x] = PointF((float)x, (float)((h - 1) * (1 - (values[x] - minVal) / (maxVal - minVal))));
				}
				float y_zero = (float)((h - 1) * (1 - (0 - minVal) / (maxVal - minVal)));
	
				// Dessiner
				g->DrawLine(Pens::Gray, (float)0, y_zero, (float)(w - 1), y_zero);
				g->DrawLines(gcnew Pen(Color::DarkRed, 3), points);
	
				// Retourner
				return ans;
	
			}
	
		// A implémenter
		protected:
			virtual double at_internal(double time01) = 0;
	
	};
	
	// Diverses implémentations
	public ref class TimeVariable_ConstMulti : public TimeVariable {
				
		// Champs
		private:
			int               _nb    ; // Nombre de valeurs (au moins une)
			array<double, 1>^ _values; // Valeurs successives prises au cours de l'évolution temporelle
			array<double, 1>^ _times ; // Instants correspondant aux valeurs, classés, allant de 0 à 1 (un de plus que le nombre de valeurs)
			
		// Constructeurs
		public:
			TimeVariable_ConstMulti(array<double, 1>^ times, array<double, 1>^ values) {
				
				// Variables nulles ?
				if (times == nullptr || values == nullptr) throw gcnew ArgumentNullException();
	
				// Nombre de valeurs
				int nb = values->Length;
				if (nb < 1) throw gcnew Exception("There should be at least one value.");
				if (times->Length != nb + 1) throw gcnew Exception("There should be one more time than the number of values.");
	
				// Temps bien classés et entre 0 et 1 ?
				if (times[0] != 0 || times[nb - 1] != 1) throw gcnew Exception("The first time should be 0 and the last time should be 1.");
				for (int i = 0; i < nb - 2; i++) {
					if (times[i + 1] < times[i]) throw gcnew Exception("The times should be sorted.");
				}
	
				// Initialiser
				initialize(nb, copy(times), copy(values));
			
			}
			TimeVariable_ConstMulti(array<double, 1>^ values) {
			
				// Variables nulles ?
				if (values == nullptr) throw gcnew ArgumentNullException();
			
				// Nombre de valeurs
				int nb = values->Length;
				if (nb < 1) throw gcnew Exception("There should be at least one value.");
	
				// Temps régulièrement espacés par défaut
				array<double, 1>^ times = gcnew array<double, 1>(nb + 1);
				for (int i = 0; i <= nb; i++) {
					times[i] = ((double)i) / nb;
				}
	
				// Initialiser
				initialize(nb, times, copy(values));
			
			}
		public:
			TimeVariable_ConstMulti(double value1) {
	
				initialize(
					1,
					gcnew array<double, 1> { 0     , 1 },
					gcnew array<double, 1> { value1 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2) {
	
				initialize(
					2,
					gcnew array<double, 1> { 0     , 0.5   , 1 },
					gcnew array<double, 1> { value1, value2 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3) {
	
				initialize(
					3,
					gcnew array<double, 1> { 0     , 1. / 3, 2. / 3, 1 },
					gcnew array<double, 1> { value1, value2, value3 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4) {
	
				initialize(
					4,
					gcnew array<double, 1> { 0     , 0.25  , 0.5   , 0.75  , 1 },
					gcnew array<double, 1> { value1, value2, value3, value4 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4, double value5) {
	
				initialize(
					5,
					gcnew array<double, 1> { 0     , 0.2   , 0.4   , 0.6   , 0.8   , 1 },
					gcnew array<double, 1> { value1, value2, value3, value4, value5 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4, double value5, double value6) {
	
				initialize(
					6,
					gcnew array<double, 1> { 0     , 1. / 6, 2. / 6, 3. / 6, 4. / 6, 5. / 6, 1 },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7) {
	
				initialize(
					7,
					gcnew array<double, 1> { 0     , 1. / 7, 2. / 7, 3. / 7, 4. / 7, 5. / 7, 6. / 7, 1 },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8) {
	
				initialize(
					8,
					gcnew array<double, 1> { 0     , 1. / 8, 2. / 8, 3. / 8, 4. / 8, 5. / 8, 6. / 8, 7. / 8, 1 },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8, double value9) {
	
				initialize(
					9,
					gcnew array<double, 1> { 0     , 1. / 9, 2. / 9, 3. / 9, 4. / 9, 5. / 9, 6. / 9, 7. / 9, 8. / 9, 1 },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8, value9 }
				);
			
			}
			TimeVariable_ConstMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8, double value9, double value10) {
	
				initialize(
					10,
					gcnew array<double, 1> { 0     , 0.1   , 0.2   , 0.3   , 0.4   , 0.5   , 0.6   , 0.7   , 0.8   , 0.9    , 1 },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8, value9, value10 }
				);
			
			}
		private:
			void initialize(int nb, array<double, 1>^ timesCopy, array<double, 1>^ valuesCopy) {
				
				_nb     = nb        ;
				_values = valuesCopy;
				_times  = timesCopy ;
	
			}
			
		// Implémentation
		public:
			virtual double at_internal(double time01) override {
	
				// Localiser 'time01' entre deux valeurs du tableau '_times' (recherche par dichotomie)
				int index; {
					int index_min = 0;
					int index_max = _nb;
					while (index_min != index_max) {
						int midIndex = (index_min + index_max + 1) / 2;
						double midTime = _times[midIndex];
						if (time01 <= midTime) index_max = midIndex - 1;
						else                    index_min = midIndex    ;
					}
					index = index_min;
				}
				
				// Retourner
				return _values[index];
			
			}
			
	};
	public ref class TimeVariable_ConstTwice : public TimeVariable_ConstMulti {
					
		// Constructeurs
		public:
			TimeVariable_ConstTwice(double valBeg, double valEnd) :
				TimeVariable_ConstMulti(valBeg, valEnd)
			{ }
			TimeVariable_ConstTwice(double valBeg, double valEnd, double timeCut) :
				TimeVariable_ConstMulti(newArray(0., timeCut, 1.), newArray(valBeg, valEnd))
			{ }
			
	};
	public ref class TimeVariable_Const : public TimeVariable {
				
		// Champs
		private:
			double _val;
				
		// Constructeurs
		public:
			TimeVariable_Const(double val) :
				_val(val)
			{ }
				
		// Implémentation
		public:
			virtual double at_internal(double time01) override {
						
				return _val;
			
			}
				
	};
	public ref class TimeVariable_LinMulti : public TimeVariable {
				
		// Champs
		private:
			int               _nb    ; // Nombre de valeurs (au moins deux, pour le début et la fin)
			array<double, 1>^ _values; // Valeurs successives prises au cours de l'évolution temporelle
			array<double, 1>^ _times ; // Instants correspondant aux valeurs, classés, allant de 0 à 1
			
		// Constructeurs
		public:
			TimeVariable_LinMulti(array<double, 1>^ times, array<double, 1>^ values) {
				
				// Variables nulles ?
				if (times == nullptr || values == nullptr) throw gcnew ArgumentNullException();
	
				// Nombre de valeurs
				int nb = values->Length;
				if (nb < 2) throw gcnew Exception("There should be at least 2 values.");
				if (times->Length != nb) throw gcnew Exception("There should be the same number of values and times.");
	
				// Temps bien classés et entre 0 et 1 ?
				if (times[0] != 0 || times[nb - 1] != 1) throw gcnew Exception("The first time should be 0 and the last time should be 1.");
				for (int i = 0; i < nb - 2; i++) {
					if (times[i + 1] < times[i]) throw gcnew Exception("The times should be sorted.");
				}
	
				// Initialiser
				initialize(nb, copy(times), copy(values));
			
			}
			TimeVariable_LinMulti(array<double, 1>^ values) {
			
				// Variables nulles ?
				if (values == nullptr) throw gcnew ArgumentNullException();
			
				// Nombre de valeurs
				int nb = values->Length;
				if (nb < 2) throw gcnew Exception("There should be at least 2 values.");
	
				// Temps régulièrement espacés par défaut
				array<double, 1>^ times = gcnew array<double, 1>(nb);
				for (int i = 0; i < nb; i++) {
					times[i] = ((double)i) / (nb - 1);
				}
	
				// Initialiser
				initialize(nb, times, copy(values));
			
			}
		public:
			TimeVariable_LinMulti(double value1, double value2) {
	
				initialize(
					2,
					gcnew array<double, 1> { 0     , 1      },
					gcnew array<double, 1> { value1, value2 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3) {
	
				initialize(
					3,
					gcnew array<double, 1> { 0     , 0.5   , 1      },
					gcnew array<double, 1> { value1, value2, value3 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4) {
	
				initialize(
					4,
					gcnew array<double, 1> { 0     , 1. / 3, 2. / 3, 1      },
					gcnew array<double, 1> { value1, value2, value3, value4 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5) {
	
				initialize(
					5,
					gcnew array<double, 1> { 0     , 0.25  , 0.5   , 0.75  , 1      },
					gcnew array<double, 1> { value1, value2, value3, value4, value5 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5, double value6) {
	
				initialize(
					6,
					gcnew array<double, 1> { 0     , 0.2   , 0.4   , 0.6   , 0.8   , 1      },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7) {
	
				initialize(
					7,
					gcnew array<double, 1> { 0     , 1. / 6, 2. / 6, 3. / 6, 4. / 6, 5. / 6, 1      },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8) {
	
				initialize(
					8,
					gcnew array<double, 1> { 0     , 1. / 7, 2. / 7, 3. / 7, 4. / 7, 5. / 7, 6. / 7, 1      },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8, double value9) {
	
				initialize(
					9,
					gcnew array<double, 1> { 0     , 1. / 8, 2. / 8, 3. / 8, 4. / 8, 5. / 8, 6. / 8, 7. / 8, 1      },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8, value9 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8, double value9, double value10) {
	
				initialize(
					10,
					gcnew array<double, 1> { 0     , 1. / 9, 2. / 9, 3. / 9, 4. / 9, 5. / 9, 6. / 9, 7. / 9, 8. / 9, 1       },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8, value9, value10 }
				);
			
			}
			TimeVariable_LinMulti(double value1, double value2, double value3, double value4, double value5, double value6, double value7, double value8, double value9, double value10, double value11) {
	
				initialize(
					11,
					gcnew array<double, 1> { 0     , 0.1   , 0.2   , 0.3   , 0.4   , 0.5   , 0.6   , 0.7   , 0.8   , 0.9    , 1       },
					gcnew array<double, 1> { value1, value2, value3, value4, value5, value6, value7, value8, value9, value10, value11 }
				);
			
			}
		private:
			void initialize(int nb, array<double, 1>^ timesCopy, array<double, 1>^ valuesCopy) {
				
				_nb     = nb        ;
				_values = valuesCopy;
				_times  = timesCopy ;
	
			}
			
		// Implémentation
		public:
			virtual double at_internal(double time01) override {
	
				// Localiser 'time01' entre deux valeurs du tableau '_times' (recherche par dichotomie)
				int index; {
					int index_min = 0;
					int index_max = _nb - 1;
					while (index_min != index_max) {
						int midIndex = (index_min + index_max + 1) / 2;
						double midTime = _times[midIndex];
						if (time01 <= midTime) index_max = midIndex - 1;
						else                     index_min = midIndex    ;
					}
					index = index_min;
				}
				
				// Placer les bornes du segment linéaire
				double timeBeg = _times [index    ];
				double timeEnd = _times [index + 1];
				double valBeg  = _values[index    ];
				double valEnd  = _values[index + 1];
				
				// Cas particulier ?
				if (timeBeg == timeEnd) return valBeg;
				
				// Retourner
				return valBeg + (time01 - timeBeg) * (valEnd - valBeg) / (timeEnd - timeBeg);
			
			}
			
	};
	public ref class TimeVariable_LinTwice : public TimeVariable_LinMulti {
					
		// Constructeurs
		public:
			TimeVariable_LinTwice(double valBeg, double valMid, double valEnd) :
				TimeVariable_LinMulti(valBeg, valMid, valEnd)
			{ }
			TimeVariable_LinTwice(double valBeg, double valMid, double timeMid, double valEnd) :
				TimeVariable_LinMulti(newArray(0., timeMid, 1.), newArray(valBeg, valMid, valEnd))
			{ }
			
	};
	public ref class TimeVariable_Lin : public TimeVariable {
				
		// Champs
		private:
			double _valBeg;
			double _valEnd_minus_valBeg;
			
		// Constructeurs
		public:
			TimeVariable_Lin(double valBeg, double valEnd) :
				_valBeg(valBeg),
				_valEnd_minus_valBeg(valEnd - valBeg)
			{ }
			
		// Implémentation
		public:
			virtual double at_internal(double time01) override {
						
				return _valBeg + time01 * _valEnd_minus_valBeg;
			
			}
			
	};
	public ref class TimeVariable_Sigm : public TimeVariable {
				
		// Champs
		private:
			double _valBeg;
			double _valEnd_minus_valBeg;
			double _speedMult;
			double _sigm_beg;
			double _sigm_end;
			
		// Constructeurs
		public:
			TimeVariable_Sigm(double valBeg, double valEnd, double speedMult) :
				_valBeg             (valBeg              ),
				_valEnd_minus_valBeg(valEnd - valBeg     ),
				_speedMult          (speedMult           ),
				_sigm_beg           (sigmAt(0, speedMult)),
				_sigm_end           (sigmAt(1, speedMult))
			{ }
			TimeVariable_Sigm(double valBeg, double valEnd) :
				_valBeg             (valBeg         ),
				_valEnd_minus_valBeg(valEnd - valBeg),
				_speedMult          (1              ),
				_sigm_beg           (sigmAt(0, 1)   ),
				_sigm_end           (sigmAt(1, 1)   )
			{ }
			
		// Implémentation
		public:
			virtual double at_internal(double time01) override {
				
				// Calculer la valeur de la sigmoïde à l'instant demandé
				double sigm_now = sigmAt(time01, _speedMult);
	
				// Normaliser et retourner
				return _valBeg + (sigm_now - _sigm_beg) * _valEnd_minus_valBeg / (_sigm_end - _sigm_beg);
	
			}
		private:
			static double sigmAt(double time01, double speedMult) {
				
				return Math::Tanh((time01 * 2 - 1) * speedMult);
	
			}
				
	};
	public ref class TimeVariable_Exp : public TimeVariable {
				
		// Champs
		private:
			double _valBeg;
			double _ln_valEnd_div_valBeg;
				
		// Constructeurs
		public:
			TimeVariable_Exp(double valBeg, double valEnd) :
				_valBeg(valBeg),
				_ln_valEnd_div_valBeg(Math::Log(valEnd / valBeg))
			{ }
				
		// Implémentation
		public:
			virtual double at_internal(double time01) override {
						
				return _valBeg * Math::Exp(_ln_valEnd_div_valBeg * time01);
			
			}
				
	};

}
}
