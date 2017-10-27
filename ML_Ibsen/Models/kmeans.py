import numpy as np
from sklearn.cluster import KMeans


def group_shots(shots, n):
    trains = []
    for c, shot in enumerate(shots):
        x = np.zeros(len(shot))
        y = np.zeros(len(shot))
        z = np.zeros(len(shot))
        rx = np.zeros(len(shot))
        for i, point in enumerate(shot):
            # x
            x[i] = point[0]
            y[i] = point[1]
            z[i] = point[2]
            rx[i] = point[3]

        x = np.array(x)
        y = np.array(y)
        z = np.array(z)
        rx = np.array(rx)
        kmeans_x = KMeans(n_clusters=n, random_state=1).fit(x.reshape(-1, 1))
        kmeans_y = KMeans(n_clusters=n, random_state=1).fit(y.reshape(-1, 1))
        kmeans_z = KMeans(n_clusters=n, random_state=1).fit(z.reshape(-1, 1))
        kmeans_rx = KMeans(n_clusters=n, random_state=1).fit(rx.reshape(-1, 1))
        trains.append(np.array([kmeans_x, kmeans_y, kmeans_z, kmeans_rx]))
    return trains


def get_center_cords(shots, n):
    # each kmeans has n clusers
    input_neurons_shots = np.zeros((len(shots), n))
    for i, group in enumerate(shots):
        input_neurons_shot = np.zeros(n)
        for clusters in group:
            cluster_centers = clusters.cluster_centers_
            for j, center in enumerate(cluster_centers):
                input_neurons_shot[j] = center[0]
        input_neurons_shots[i] = input_neurons_shot
    return np.array(input_neurons_shots)