# BSG for CLP

BSG is a method based on beam search(an adaptation of a branch-and-bound algorithm which expands only the most promising nodes at each level of the search tree). BSG explores the space to find a path from an it initial state (i.e., an empty container) to some complete solution or it terminal state (i.e., a loading plan in which no more boxes can be loaded).

## Compiling

```sh
git clone https://github.com/lucasguesserts/Metasolver.git
cd Metasolver
make
./build/Release/bsg --help
```

This will generate the `bsg` (`build/Release/bsg`).

By default, it compiles in Release mode. To compile in Debug mode, run `make BUILD_TYPE=Debug` (the binary will be `build/Debug/bsg`).

## `bsg` Example

```sh
./build/Release/bsg --help # for all options
$ ./build/Release/bsg instances/BR2.txt -i 47 -t 10
***** Creando el contenedor ****
File(BR): instances/BR2.txt
Instance:48
min_fr:0.98
Maxtime:10
587 233 220
n_blocks:10000
greedy
bsg
double effort
copying state
running
beams/max_level_size:4/16
[BSG_path] new best_solution_found (0.22): 0.928765 11 nodes
[BSG_path] new best_solution_found (0.32): 0.9317344 6 nodes
[BSG_path] new best_solution_found (0.45): 0.93572355 13 nodes
[BSG_path] new best_solution_found (0.47): 0.94048782 15 nodes
[BSG_path] new best_solution_found (0.48): 0.94283454 14 nodes
[BSG_path] new best_solution_found (0.49): 0.94744636 16 nodes
[DoubleEffort] new best_solution_found (0.51): 0.94744636
beams/max_level_size:6/36
[BSG_path] new best_solution_found (0.92): 0.95853155 8 nodes
[BSG_path] new best_solution_found (1.37): 0.96328332 13 nodes
[BSG_path] new best_solution_found (1.38): 0.96447539 10 nodes
[BSG_path] new best_solution_found (1.38): 0.96525303 10 nodes
[DoubleEffort] new best_solution_found (1.39): 0.96525303
beams/max_level_size:8/64
beams/max_level_size:11/121
[BSG_path] new best_solution_found (6.03): 0.96556091 10 nodes
[DoubleEffort] new best_solution_found (6.06): 0.96556091
beams/max_level_size:16/256
allocation time: 10.04
wall time: 10.05
greedy search calls: 2920
% volume utilization
96.556091
```

## Instance sets

The [instance set](./instances/) has been proposed by Bischoff and Ratcliff (1995) and expanded by Davies and Bischoff (1999). It is composed of 16 classes, BR0 to BR15, with 100 instances each, totaling 1,600, organized in growing heterogeneity: the BR0 instances class has one item type (homogeneous); BR1 to BR7 have few item types (weakly heterogeneous) with large quantities; BR8 to BR15 have many item types (strongly heterogeneous) with small quantities. They all use a 20 ft ISO container, which has 587 cm of length, 233 cm of width, and 220 cm of height.

## Experimental Result

[There is a spreadsheet with the data of the experiments of Araya et al. 2017](./docs/results_paper_araya_2017.ods).

The authors also made the spreadsheets available online:

* [Results of BSG on the classic BR instances](https://docs.google.com/spreadsheets/u/1/d/e/2PACX-1vTp6t3tOLYOzoEXScyt5GEJxhu7oML3eq2yyX0J_t6YiMrTTf6XsXhkSsqJ5AxGkFjYBfUyl_vlLsAe/pubhtml?gid=1713343267&single=true);
* [Results of experiments on BRwp1.0-0.5 instances](https://docs.google.com/spreadsheets/d/e/2PACX-1vSk0_iexrd_v7x-I_72Albt21t9iLka3o9CUatUYh0jcxl7O-Lw_dLpJ9FRF0GdlqQ7AyrCFXoCF5wX/pubhtml?gid=1240292960&single=true);
* [Results of experiments on NMFTA instances](https://docs.google.com/spreadsheets/d/e/2PACX-1vSk0_iexrd_v7x-I_72Albt21t9iLka3o9CUatUYh0jcxl7O-Lw_dLpJ9FRF0GdlqQ7AyrCFXoCF5wX/pubhtml?gid=157777762&single=true);

## Related Literature

* BSG Heuristic:
  * [Araya, I., & Riff, M.C. (2014). A beam search approach to the container loading problem. Comput. Oper. Res., 43, 100-107.](https://doi.org/10.1016/j.cor.2013.09.003)
  * [Araya, I., Guerrero, K., & Nuñez, E. (2017). VCS: A new heuristic function for selecting boxes in the single container loading problem. Comput. Oper. Res., 82, 27-35.](https://doi.org/10.1016/J.COR.2017.01.002)
  * [Araya, I., Moyano, M., & Sanchez, C. (2020). A beam search algorithm for the biobjective container loading problem. Eur. J. Oper. Res., 286, 417-431.](https://doi.org/10.1016/j.ejor.2020.03.040)
* Instances:
  * [Bischoff, E.E., & Ratcliff, M.S. (1995). Issues in the development of approaches to container loading. Omega-international Journal of Management Science, 23, 377-390.](https://doi.org/10.1016/0305-0483%2895%2900015-G)
  * [Davies, A., & Bischoff, E.E. (1999). Weight distribution considerations in container loading. Eur. J. Oper. Res., 114, 509-527.](https://doi.org/10.1016/S0377-2217%2898%2900139-8)
