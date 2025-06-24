# ESPECIFICA++

This project develops **ESPECIFICA++** (Executable Specifications in C++), an embedded DSL in C++ that enables writing executable program specifications. It uses notation based on predicate calculus, along with basic C++ expressions and C++ functions as an abstraction mechanism.

## Project structure

- The **operational version** of the language is located in the file [`especificapp.v2/especificapp.h`](especificapp.v2/especificapp.h). This file includes:
  1. A set of macros to express quantifications and other basic constructions.
  2. An execution engine for those quantifications and constructions.

- The file [`especificapp.pdf`](especificapp.pdf) contains a **presentation of ESPECIFICA++** prepared for a workshop with students from Universidad Complutense de Madrid (UCM). This documentation is written in Spanish. The file [`INNOVA_FDI_25_JLSR.pdf`](INNOVA_FDI_25_JLSR.pdf) contains the presentation of ESPECIFICA++ in the educational innovation day, Facultad de Informática at UCM. 

- The folder [`problemas`](problemas) contains two collections of problems to solve using ESPECIFICA++ (also in Spanish).

> A more complete documentation in English will be available soon.

- The folder [`especificapp.v1`](especificapp.v1/) is included **for historical reasons** only. It contains the implementation described in the following publication:
  - [DOI: 10.1109/EDUNINE62377.2025.10981333](https://doi.org/10.1109/EDUNINE62377.2025.10981333). It also contains an extended version of this publication, with a more detailed description of this first version of ESPECIFICA++, and a poster (in Spanish) presented at [CIDICO 24](https://cidico.es/6/).

---

Thank you for your interest in ESPECIFICA++. Feedback and suggestions are very welcome.
