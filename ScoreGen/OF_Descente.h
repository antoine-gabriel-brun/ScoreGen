#pragma region head

	#pragma once

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	#include "OptionsForm.h"

#pragma endregion

namespace ScoreGen {
	public ref class OF_Descente : public OptionsForm {

	// Types
	public:
		ref struct NNote {

			public:
				int pitch;
				double moment;

			public:
				NNote(int pitch, double moment) :
					pitch(pitch),
					moment(moment)
				{ }

		};

	// Constructeur, destructeur
	public:
		OF_Descente(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_Descente() {

			if (components) delete components;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {

			return gcnew array<Object^, 1> {
				calcNotes(
					(int   )(numericUpDownNote1->Value),
					(int   )(numericUpDownNote2->Value),
					(int   )(numericUpDownDur  ->Value),
					(double)(numericUpDownExp  ->Value) * 0.01
				)
			};

		}

	// Calcul des notes
	private:
		static array<List<NNote^>^, 1>^ calcNotes(int note1, int note2, int dur, double exp) {

			int nbNotes = Math::Abs(note2 - note1) + 1;
			
			// Calculer les hauteurs de notes (suite chromatique entre note1 et note2) et l'instant de chacune des notes
			array<int, 1>^ notes = getNotes(note1, note2, nbNotes);
			array<double, 1>^ moments = getMoments(notes, nbNotes, note1, note2, dur, exp);

			// Répartir les notes aux quatre instruments
			array<List<NNote^>^, 1>^ ans = gcnew array<List<NNote^>^, 1>(4) { gcnew List<NNote^>, gcnew List<NNote^>, gcnew List<NNote^>, gcnew List<NNote^> };
			for (int i = 0; i < nbNotes; i++) {
				int pitch = notes[i];
				NNote^ note = gcnew NNote(pitch, moments[i]);
				ans[0]->Add(note); // PROV : seulement v1
				//if (pitch >= 79               ) ans[0]->Add(note);
				//if (pitch >= 65 && pitch <= 79) ans[1]->Add(note);
				//if (pitch >= 51 && pitch <= 65) ans[2]->Add(note);
				//if (               pitch <= 51) ans[3]->Add(note);
			}

			// Retourner
			return ans;

		}

	// Fonctions
	private:
		static array<int, 1>^ getNotes(int note1, int note2, int nbNotes) {
			
			array<int, 1>^ ans = gcnew array<int, 1>(nbNotes);

			int increment = note1 < note2 ? 1 : -1;
			for (int i = 0; i < nbNotes; i++) {
				ans[i] = note1 + i * increment;
			}

			return ans;

		}
		static array<double, 1>^ getMoments(array<int, 1>^ notes, int nbNotes, int note1, int note2, int dur, double exp) {
			
			array<double, 1>^ ans = gcnew array<double, 1>(nbNotes);

			for (int i = 0; i < nbNotes; i++) {
				ans[i] = getMoment(notes[i], note1, note2, dur, exp);
			}

			return ans;

		}
		static double getMoment(double note, double note1, double note2, double dur, double exp) {
			
			if (exp == 0.5) {
				return (dur - 1) * (note - note1) / (note2 - note1);
			} else {
				double rr = exp / (1 - exp);
				return 0.5 * (dur - 1) * Math::Log(1 - (note1 - note) * (1 - rr * rr) / (note1 - note2), rr);
			}

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::NumericUpDown^  numericUpDownDur;
		private: System::Windows::Forms::Label^  label3;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNote1;
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNote2;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownExp;
		private: System::Windows::Forms::Label^  label4;
		private:
				System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNote1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			this->numericUpDownExp = (gcnew System::Windows::Forms::NumericUpDown());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownDur = (gcnew System::Windows::Forms::NumericUpDown());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNote2 = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNote1))->BeginInit();
			this->groupBoxOptions->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownExp))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDur))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNote2))->BeginInit();
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(52, 25);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(111, 17);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Note de départ :";
			// 
			// numericUpDownNote1
			// 
			this->numericUpDownNote1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNote1->Location = System::Drawing::Point(183, 23);
			this->numericUpDownNote1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {108, 0, 0, 0});
			this->numericUpDownNote1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {24, 0, 0, 0});
			this->numericUpDownNote1->Name = L"numericUpDownNote1";
			this->numericUpDownNote1->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNote1->TabIndex = 5;
			this->numericUpDownNote1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {85, 0, 0, 0});
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->numericUpDownExp);
			this->groupBoxOptions->Controls->Add(this->label4);
			this->groupBoxOptions->Controls->Add(this->numericUpDownDur);
			this->groupBoxOptions->Controls->Add(this->label3);
			this->groupBoxOptions->Controls->Add(this->numericUpDownNote2);
			this->groupBoxOptions->Controls->Add(this->label1);
			this->groupBoxOptions->Controls->Add(this->numericUpDownNote1);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(338, 153);
			this->groupBoxOptions->TabIndex = 6;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// numericUpDownExp
			// 
			this->numericUpDownExp->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownExp->DecimalPlaces = 3;
			this->numericUpDownExp->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->numericUpDownExp->Location = System::Drawing::Point(183, 107);
			this->numericUpDownExp->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {99999, 0, 0, 196608});
			this->numericUpDownExp->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 196608});
			this->numericUpDownExp->Name = L"numericUpDownExp";
			this->numericUpDownExp->Size = System::Drawing::Size(103, 22);
			this->numericUpDownExp->TabIndex = 11;
			this->numericUpDownExp->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {84078, 0, 0, 196608});
			// 
			// label4
			// 
			this->label4->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(52, 109);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(74, 17);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Exposant :";
			// 
			// numericUpDownDur
			// 
			this->numericUpDownDur->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownDur->Location = System::Drawing::Point(183, 79);
			this->numericUpDownDur->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {8193, 0, 0, 0});
			this->numericUpDownDur->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {17, 0, 0, 0});
			this->numericUpDownDur->Name = L"numericUpDownDur";
			this->numericUpDownDur->Size = System::Drawing::Size(103, 22);
			this->numericUpDownDur->TabIndex = 9;
			this->numericUpDownDur->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {58, 0, 0, 0});
			// 
			// label3
			// 
			this->label3->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(52, 81);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(64, 17);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Battues :";
			// 
			// numericUpDownNote2
			// 
			this->numericUpDownNote2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNote2->Location = System::Drawing::Point(183, 51);
			this->numericUpDownNote2->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {108, 0, 0, 0});
			this->numericUpDownNote2->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {24, 0, 0, 0});
			this->numericUpDownNote2->Name = L"numericUpDownNote2";
			this->numericUpDownNote2->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNote2->TabIndex = 7;
			this->numericUpDownNote2->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {51, 0, 0, 0});
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(52, 53);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(105, 17);
			this->label1->TabIndex = 6;
			this->label1->Text = L"Note d\'arrivée :";
			// 
			// OF_Descente
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(362, 287);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_Descente";
			this->Text = L"OF_Echange";
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNote1))->EndInit();
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownExp))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownDur))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNote2))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
