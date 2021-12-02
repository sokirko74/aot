#include "morph_dict/LemmatizerBaseLib/Predict.h"
#include "morph_dict/MorphWizardLib/wizard.h"
#include "morph_dict/LemmatizerBaseLib/MorphDictBuilder.h"
#include "morph_dict/LemmatizerBaseLib/Lemmatizers.h"

#include "morph_dict/common/argparse.h"

void PrintUsage()
{
    printf("Dialing Morphological Generator (from text format (*.mrd) to binary(*.bin))\n");
    printf("Usage: MorphGen <mwz-file> OutputCatalog <PostfixLength> <MinFreq> [--allow-russian-jo]\n");
    printf("where <mwz-file> is a morph_dict project file\n");
    printf("<PostfixLength> is the length of the predicting postfix (can be 1,2,3,4,5). \n");
    printf("     By these postfixes will be unkonwn words predicted\n");
    printf("     The size of the prediction base depends crucialy upon this value\n");
    printf("<MinFreq> is the minimal frequence of the predicting postfix in the dictionary\n");
    printf("if --allow-russian-jo is swithched, then the program does not convert Russian 'jo' to 'e'\n");
    printf("if --allow-russian-jejo is swithched, then the program supports both 'e' and 'jo'\n");

    printf("Example: MorphGen rus.mwz c:/RUS_BIN/ 3 2\n");
    exit(1);
};


void initArgParser(int argc, const char** argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--input", "input file");
    parser.AddArgument("--output-folder", "output folder");
    parser.AddArgument("--postfix-len", "postfix len");
    parser.AddArgument("--min-freq", "min freq");
    parser.AddOption("--allow-russian-jo");
    parser.AddOption("--allow-russian-jejo");
    parser.Parse(argc, argv);
}


int main(int argc, const char* argv[])
{
    ArgumentParser args;
    initArgParser(argc, argv, args);
    bool bAllowRussianJeJo = args.Exists("allow-russian-jejo");
    bool bAllowRussianJo = bAllowRussianJeJo || args.Exists("allow-russian-jo");
    int PostfixLength = atoi(args.Retrieve("postfix-len").c_str());
    if ((PostfixLength == 0) || (PostfixLength > 5))
    {
        std::cerr << "PostfixLength is std::set to " << PostfixLength << "\n";
        std::cerr << "PostfixLength should be between 1 and 5\n";
        return 1;
    };

    int MinFreq = atoi(args.Retrieve("min-freq").c_str());
    if (MinFreq <= 0) {
        std::cerr << "MinFreq should be more than 0\n";
        return 1;
    };

    std::string  Error;
    if (!IsRmlRegistered(Error)) {
        std::cerr << Error << "\n";
        return 1;
    };

    MorphLanguageEnum wizardLangua;
    try {

        //  adding "/" to the end of  OutputPath
        std::string OutputPath = args.Retrieve("output");
        if (!OutputPath.empty())
        {
            char c = OutputPath[OutputPath.length() - 1];
            if ((c != '/')
                && (c != '\\')
                )
                OutputPath += '/';
        };

        MorphoWizard Wizard;
        std::string WizardFilename = argv[1];
        Wizard.load_wizard(args.Retrieve("input").c_str(), "guest", false);
        wizardLangua = Wizard.m_Language;
        if (!bAllowRussianJo)
        {
            std::cerr << "prepare_for_RML\n";
            if (!Wizard.prepare_for_RML())
                return 1;
        };
        if (bAllowRussianJeJo)
        {
            std::cerr << "prepare_for_RML2\n";
            if (!Wizard.prepare_for_RML2())
                return 1;
        };

        {
            CMorphDictBuilder R(Wizard.m_Language);
            R.GenerateLemmas(Wizard);
            R.GenerateUnitedFlexModels(Wizard);
            R.CreateAutomat(Wizard);
            std::cerr << "Saving...\n";
            std::string outFileName = std::string(OutputPath + MORPH_MAIN_FILES);
            if (!R.Save(outFileName))
            {
                std::cerr << "Cannot save the main automat to " << outFileName << std::endl;
                return 1;
            };
            std::cerr << "Successful written indices of the main automat to " << outFileName << std::endl;

            if (!R.GenPredictIdx(Wizard, PostfixLength, MinFreq, OutputPath))
            {
                std::cerr << "Cannot create prediction base\n";
                return 1;
            };
        }


        { // generating options file
            std::string OptFileName = OutputPath + OPTIONS_FILE;
            std::cerr << "writing options file " << OptFileName << std::endl;
            FILE* opt_fp = fopen(OptFileName.c_str(), "w");
            if (!opt_fp)
            {
                std::cerr << "Cannot write to file " << OptFileName << std::endl;
                return 1;
            };
            if (bAllowRussianJo)
                fprintf(opt_fp, "AllowRussianJo\n");
            fclose(opt_fp);
            std::cerr << "Options file was created\n";
        }

    }
    catch (CExpc e)
    {
        std::cerr << "exception=" << e.m_strCause << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Can not Generate,  general exception\n";
        return 1;
    }
    std::cerr << "exit with success\n";
    return 0;
}


