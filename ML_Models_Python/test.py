import unittest
from Data_Processing.generate_labels import generate_labels
from Data_Classes.classes import LabelKind


class Test_Generate_Labels(unittest.TestCase):
    labels = ["Mathias", "Morten", "Anton", "Morten"]

    def test_ordinary_labelKind(self):
        new_labels, label_dict = generate_labels(self.labels, LabelKind.Ordinary)
        self.assertEqual(new_labels, [0, 1, 2, 1])
        self.assertEqual(label_dict[0], "Mathias")
        self.assertEqual(label_dict[1], "Morten")
        self.assertEqual(label_dict[2], "Anton")

    def test_arbitrary_labelKind(self):
        new_labels, label_dict = generate_labels(self.labels, LabelKind.Arbitrary)
        self.assertEqual(new_labels, [[0, 1, 0], [0, 0, 1], [1, 0, 0], [0, 0, 1]])
        self.assertEqual(label_dict[''.join(str(nr) for nr in new_labels[0])], "Mathias")
        self.assertEqual(label_dict[''.join(str(nr) for nr in new_labels[1])], "Morten")
        self.assertEqual(label_dict[''.join(str(nr) for nr in new_labels[2])], "Anton")
        self.assertEqual(label_dict[''.join(str(nr) for nr in new_labels[3])], "Morten")


if __name__ == '__main__':
    unittest.main()