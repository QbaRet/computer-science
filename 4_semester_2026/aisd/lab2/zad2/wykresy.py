import pandas as pd
import matplotlib.pyplot as plt
import os

def generate_plots_split(k):
    file_name = f'wyniki_k{k}.csv'
    if not os.path.exists(file_name):
        print(f"Brak pliku {file_name}, pomijam...")
        return

    df = pd.read_csv(file_name, sep=';', names=['algo', 'n', 'c', 's'])

    avg = df.groupby(['algo', 'n']).mean().reset_index()
    data_ins = avg[avg['algo'] == 'InsertionSort']
    
    if not data_ins.empty:
        fig_ins, axs_ins = plt.subplots(2, 2, figsize=(12, 8))
        fig_ins.suptitle(f'Analiza Insertion Sort (Małe Dane) - k = {k}', fontsize=16)
        color = 'red'

        axs_ins[0, 0].plot(data_ins['n'], data_ins['c'], marker='o', label='InsertionSort', color=color)
        axs_ins[0, 0].set_title('Porównania c(n)')
        axs_ins[0, 0].set_ylabel('c')
        
        axs_ins[0, 1].plot(data_ins['n'], data_ins['s'], marker='o', label='InsertionSort', color=color)
        axs_ins[0, 1].set_title('Przestawienia s(n)')
        axs_ins[0, 1].set_ylabel('s')

        axs_ins[1, 0].plot(data_ins['n'], data_ins['c'] / data_ins['n'], marker='o', label='InsertionSort', color=color)
        axs_ins[1, 0].set_title('Iloraz c/n')
        axs_ins[1, 0].set_ylabel('c/n')

        axs_ins[1, 1].plot(data_ins['n'], data_ins['s'] / data_ins['n'], marker='o', label='InsertionSort', color=color)
        axs_ins[1, 1].set_title('Iloraz s/n')
        axs_ins[1, 1].set_ylabel('s/n')

        for ax in axs_ins.flat:
            ax.set_xlabel('n')
            ax.legend()
            ax.grid(True)

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])        

        ins_plot_name = f'wykresy_k{k}_ins_small.png'
        plt.savefig(ins_plot_name)
        print(f"Wygenerowano {ins_plot_name}")
        plt.close(fig_ins)
    else:
        print(f"Brak danych dla InsertionSort w {file_name}")

    data_large = avg[avg['algo'].isin(['QuickSort', 'HybridSort'])]
    
    if not data_large.empty:
        fig_large, axs_large = plt.subplots(2, 2, figsize=(15, 10))
        fig_large.suptitle(f'Porównanie QuickSort i HybridSort (Duże Dane) - k = {k}', fontsize=16)

        colors_large = {'QuickSort': 'blue', 'HybridSort': 'green'}
        algorithms_large = ['QuickSort', 'HybridSort']

        for algo in algorithms_large:
            data = data_large[data_large['algo'] == algo]
            if not data.empty:
                c_color = colors_large.get(algo, 'black')


                axs_large[0, 0].plot(data['n'], data['c'], marker='o', markersize=2, linestyle='-', label=algo, color=c_color)
                axs_large[0, 0].set_title('Porównania c(n)')
                axs_large[0, 0].set_ylabel('c')
                
                axs_large[0, 1].plot(data['n'], data['s'], marker='o', markersize=2, linestyle='-', label=algo, color=c_color)
                axs_large[0, 1].set_title('Przestawienia s(n)')
                axs_large[0, 1].set_ylabel('s')

                axs_large[1, 0].plot(data['n'], data['c'] / data['n'], marker='o', markersize=2, linestyle='-', label=algo, color=c_color)
                axs_large[1, 0].set_title('Iloraz c/n')
                axs_large[1, 0].set_ylabel('c/n')

                axs_large[1, 1].plot(data['n'], data['s'] / data['n'], marker='o', markersize=2, linestyle='-', label=algo, color=c_color)
                axs_large[1, 1].set_title('Iloraz s/n')
                axs_large[1, 1].set_ylabel('s/n')

        for ax in axs_large.flat:
            ax.set_xlabel('n')
            ax.legend()
            ax.grid(True)

        plt.tight_layout(rect=[0, 0.03, 1, 0.95])
        
        large_plot_name = f'wykresy_k{k}_large_compare.png'
        plt.savefig(large_plot_name)
        print(f"Wygenerowano {large_plot_name}")
        plt.close(fig_large)
    else:
        print(f"Brak danych dla QuickSort/HybridSort w {file_name}")


for k in [1, 10, 100]:
    generate_plots_split(k)