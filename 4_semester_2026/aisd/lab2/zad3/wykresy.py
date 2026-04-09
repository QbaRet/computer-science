import pandas as pd
import matplotlib.pyplot as plt
import os
def plot_task3(k):
    file_name = f'wyniki_zad3_k{k}.csv'
    df = pd.read_csv(file_name, sep=';', names=['algo', 'n', 'c', 's'])
    avg = df.groupby(['algo', 'n']).mean().reset_index()
    fig, axs = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle(f'Zadanie 3: ShortestPairSort vs MergeSort (k={k})', fontsize=16)


    colors = {'ShortestPairSort': '#FF8C00', 'MergeSort': '#4169E1'}

    for algo in avg['algo'].unique():
        data = avg[avg['algo'] == algo]
        c = colors.get(algo, 'black')


        axs[0, 0].plot(data['n'], data['c'], label=algo, color=c, marker='o', markersize=3)
        axs[0, 0].set_title('Liczba porównań c(n)')
        

        axs[0, 1].plot(data['n'], data['s'], label=algo, color=c, marker='o', markersize=3)
        axs[0, 1].set_title('Liczba przestawień s(n)')


        axs[1, 0].plot(data['n'], data['c'] / data['n'], label=algo, color=c, marker='o', markersize=3)
        axs[1, 0].set_title('Iloraz c/n (złożoność logarytmiczna)')


        axs[1, 1].plot(data['n'], data['s'] / data['n'], label=algo, color=c, marker='o', markersize=3)
        axs[1, 1].set_title('Iloraz s/n')


    for ax in axs.flat:
        ax.set_xlabel('n')
        ax.legend()
        ax.grid(True, linestyle='--', alpha=0.7)

    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    

    output_name = f'wykres_zad3_k{k}.png'
    plt.savefig(output_name)
    plt.show()


for k_val in [1, 10, 100]:
    plot_task3(k_val)