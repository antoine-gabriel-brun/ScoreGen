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
	public ref class OF_Test : public OptionsForm {

	// Constructeur, destructeur
	public:
		OF_Test(String^ title, String^ description) :
			OptionsForm(title, description)
		{
			
			String^ text = Text;

			InitializeComponent();

			Text = text;

		}
	protected:
		~OF_Test() {

			if (components) delete components;

		}

	// Implémentations
	protected:
		virtual array<Object^, 1>^ getOptionsInFormValues() override {

			return gcnew array<Object^, 1>{
				(int)(numericUpDownNbParts->Value),
				(int)(numericUpDownNbNotes->Value)
			};

		}

	// Contrôles
	#pragma region controls
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNbParts;
		private: System::Windows::Forms::Label^  label1;
		private: System::Windows::Forms::Label^  label2;
		private: System::Windows::Forms::NumericUpDown^  numericUpDownNbNotes;
		private: System::Windows::Forms::GroupBox^  groupBoxOptions;
		private:
				System::ComponentModel::Container ^components;
	#pragma endregion

#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->numericUpDownNbParts = (gcnew System::Windows::Forms::NumericUpDown());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDownNbNotes = (gcnew System::Windows::Forms::NumericUpDown());
			this->groupBoxOptions = (gcnew System::Windows::Forms::GroupBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbParts))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->BeginInit();
			this->groupBoxOptions->SuspendLayout();
			this->SuspendLayout();
			// 
			// numericUpDownNbParts
			// 
			this->numericUpDownNbParts->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNbParts->Location = System::Drawing::Point(187, 67);
			this->numericUpDownNbParts->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownNbParts->Name = L"numericUpDownNbParts";
			this->numericUpDownNbParts->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNbParts->TabIndex = 2;
			this->numericUpDownNbParts->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {8, 0, 0, 0});
			// 
			// label1
			// 
			this->label1->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(48, 69);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(133, 17);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Nombre de parties :";
			// 
			// label2
			// 
			this->label2->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(48, 97);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(118, 17);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Notes par partie :";
			// 
			// numericUpDownNbNotes
			// 
			this->numericUpDownNbNotes->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->numericUpDownNbNotes->Location = System::Drawing::Point(187, 95);
			this->numericUpDownNbNotes->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 0});
			this->numericUpDownNbNotes->Name = L"numericUpDownNbNotes";
			this->numericUpDownNbNotes->Size = System::Drawing::Size(103, 22);
			this->numericUpDownNbNotes->TabIndex = 5;
			this->numericUpDownNbNotes->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {20, 0, 0, 0});
			// 
			// groupBoxOptions
			// 
			this->groupBoxOptions->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->groupBoxOptions->Controls->Add(this->numericUpDownNbParts);
			this->groupBoxOptions->Controls->Add(this->numericUpDownNbNotes);
			this->groupBoxOptions->Controls->Add(this->label1);
			this->groupBoxOptions->Controls->Add(this->label2);
			this->groupBoxOptions->Location = System::Drawing::Point(12, 84);
			this->groupBoxOptions->Name = L"groupBoxOptions";
			this->groupBoxOptions->Size = System::Drawing::Size(338, 165);
			this->groupBoxOptions->TabIndex = 6;
			this->groupBoxOptions->TabStop = false;
			this->groupBoxOptions->Text = L"Options";
			// 
			// OF_Test
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(362, 299);
			this->Controls->Add(this->groupBoxOptions);
			this->Name = L"OF_Test";
			this->Text = L"OF_Test";
			this->Controls->SetChildIndex(this->groupBoxOptions, 0);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbParts))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numericUpDownNbNotes))->EndInit();
			this->groupBoxOptions->ResumeLayout(false);
			this->groupBoxOptions->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	
	};
}
