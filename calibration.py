import numpy as np
import cv2
import glob


# 0 - камера на ноуте, 1 - камера подключенная
flag = 0
video_capture = cv2.VideoCapture(flag)

# Check success
if not video_capture.isOpened():
    raise Exception("Could not open video device")

# prepare object points
nx = 9          # number of inside corners in x
ny = 6          # number of inside corners in y

# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(nx,ny,0)
objp = np.zeros((nx*ny, 3), np.float32)
objp[:, :2] = np.mgrid[0:nx, 0:ny].T.reshape(-1, 2)

# Arrays to store object points and image points from all the images.
objpoints = []      # 3d point in real world space
imgpoints = []      # 2d points in image plane.

# включаем камеру, ищем шахматы
count = 0           # счетчик сохраненных кадров
ret2 = False        # булевая переменная, сигнализирующая о том, что были увидены шахматы
while True:
    # закрыть окно камеры по нажатию клавиши 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    ret1, frame = video_capture.read()
    gray = cv2.cvtColor(frame.copy(), cv2.COLOR_BGR2GRAY)
    img = frame.copy()
    ret2, corners = cv2.findChessboardCorners(gray, (nx, ny), None)
    # если была найдена шахматка, можем сохранять кадры по нажатию клавиши 'f'
    if ret2:
        # отображать углы найденных шахматок
        corners2 = cv2.cornerSubPix(gray, corners, (19, 19), (-1, -1), criteria)
        img = cv2.drawChessboardCorners(frame, (nx, ny), corners2, ret2)
        if cv2.waitKey(1) & 0xFF == ord('f'):
            cv2.imwrite("frame" + str(count) + ".jpg", frame)
            count += 1
            print('taken frame')
    # если шахматка не найдена, продолжаем искать
    else:
        if cv2.waitKey(1) & 0xFF == ord('f'):
            print("i don't see chessboard")
    # вычисление размытости и вывод
    fm = cv2.Laplacian(frame, cv2.CV_64F).var()
    cv2.putText(img, "{:12.4f}".format(fm), (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (150, 150, 150), lineType=cv2.LINE_AA)
    cv2.imshow('frame', img)

# имена всех взятых кадров
images = glob.glob('*.jpg')

# пробегаем по кадрам, вычисляем object points (в трехмерном пространстве) и image points (в пространстве изображения)
print('started watch images')
for fname in images:
    print("next image")
    img = cv2.imread(fname)
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    ret, corners = cv2.findChessboardCorners(gray, (nx, ny), None)
    # If found, add object points, image points (after refining them)
    if ret:
        print('i see')
        objpoints.append(objp)
        corners2 = cv2.cornerSubPix(gray, corners, (19, 19), (-1, -1), criteria)
        imgpoints.append(corners2)
        # отображение углов шахматки
        # img = cv2.drawChessboardCorners(img, (nx, ny), corners2, ret)
        # cv2.imshow('img', img)

print('i had saw all images')
# вычисление mtx - матрица преобразования камеры и  dist - вектор коэффициентов искажения
ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, cv2.cvtColor(cv2.imread(images[0]), cv2.COLOR_BGR2GRAY).shape[::-1], None, None)

# writing coef into coef.txt
file = open("coef.txt", "w+")
# размерность матрицы - первые два числа
strr = str(len(mtx)) + "\n" + str(len(mtx[0])) + "\n"
# потом сама матрица соответствующей размерности
for i in range(len(mtx[0])):
    for j in range(len(mtx)):
        strr += str(mtx[i, j]) + "\n"
# размер вектора коэффициентов искажения
strr += str(len(dist[0])) + "\n"
# сам вектор искажения
for i in range(len(dist[0])):
    strr += str(dist[0, i]) + "\n"
file.write(strr)
file.close()

h, w = cv2.imread(images[0]).shape[:2]
newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))
x, y, w, h = roi

# сохраняем изображения - уже с откалиброванной камерой
count = 0       # счетчик изображений
for fname in images:
    # undistortion
    dst = cv2.undistort(cv2.imread(fname), mtx, dist, None, newcameramtx)
    # crop the image
    dst = dst[y:y + h, x:x + w]
    cv2.imwrite("calib" + str(count) + ".png", dst)
    count += 1

# запускаем камеру с применением калибровки
while True:
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    ret, frame = video_capture.read()
    # undistortion
    dst = cv2.undistort(frame, mtx, dist, None, newcameramtx)
    # crop the image
    dst = dst[y:y + h, x:x + w]
    cv2.imshow('frame', dst)

# Close device
video_capture.release()
cv2.destroyAllWindows()

