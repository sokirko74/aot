import argparse

ENTRY_DELIMITER = "============"


class TStructDict: 
    def __init__(self):
        self.entries = list()

    def read(self, path):
        entry = list()
        with open(path)  as inp:
            for l in inp:
                l = l.rstrip()
                if l == ENTRY_DELIMITER:
                    if len(entry) > 0:
                        self.entries.append(entry)
                        entry = list()
                else:
                    entry.append(l)
            if len(entry) > 0:
                self.entries.append(entry)

    def write (self, path):
        with open(path, "w") as outp:
            for e in self.entries:
                outp.write(ENTRY_DELIMITER + "\n")
                for l in e:
                    outp.write(l + "\n")

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input-file", dest='in_path', help="ross.txt")
    parser.add_argument("--output-file", dest='out_path');
    return parser.parse_args()

def process_one_value(title, s):
    if s.endswith(" ГГ"):
        return s[:-3] + " X! : ГГ"
    elif s.endswith(" ИГ"):
        return s[:-3] + " X! : ИГ"
    elif s.endswith(" ЧИСЛ"):
        return s[:-5] + " X! : ЧИСЛ"
    elif s.endswith(" ПРОП"):
        return s[:-5] + " X! : ПРОП"
    else:
        raise Exception("title = " + title + ",  unknown format " + s)


def process(entry):
    prep = False
    gf1 = True
    for index, line in enumerate(entry):
        if line.startswith("GF ") and (line.find(" ПРЕДЛ") != -1 or line.find("ПРОСТ_ПРЕДЛ") != -1):
            prep = True
        if prep and line.startswith("GF1 "):
            entry[index] = process_one_value(entry[0], entry[index])
            gf1 = True
        elif line.find('= ') != -1:
            gf1 = False
        elif gf1:
            entry[index] = process_one_value(entry[0], entry[index])

def main():
    args = parse_args()
    d = TStructDict()
    d.read(args.in_path)
    for e in d.entries:
        process(e)
    d.write(args.out_path)


if __name__ == "__main__":
    main()


