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
nx = 9  # number of inside corners in x
ny = 6  # number of inside corners in y

# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
objp = np.zeros((nx*ny, 3), np.float32)
objp[:, :2] = np.mgrid[0:nx, 0:ny].T.reshape(-1, 2)

# Arrays to store object points and image points from all the images.
objpoints = []      # 3d point in real world space
imgpoints = []      # 2d points in image plane.

count = 0
ret2 = False
while True:
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    ret1, frame = video_capture.read()
    img = cv2.cvtColor(frame.copy(), cv2.COLOR_BGR2GRAY)
    if ret2:
        if cv2.waitKey(1) & 0xFF == ord('f'):
            cv2.imwrite("frame" + str(count) + ".jpg", frame)
            count += 1
            print('taken frame')
    else:
        if cv2.waitKey(1) & 0xFF == ord('f'):
            print("i don't see chessboard")
        ret2, corners = cv2.findChessboardCorners(img, (nx, ny), None)

    fm = cv2.Laplacian(frame, cv2.CV_64F).var()
    cv2.putText(frame, "{:12.4f}".format(fm), (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1.0, (150, 150, 150), lineType=cv2.LINE_AA)
    cv2.imshow('frame', frame)

images = glob.glob('*.jpg')

print('started watch images')
count = 0
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
        # img = cv2.drawChessboardCorners(img, (nx, ny), corners2, ret)
        # cv2.imshow('img', img)

print('i had saw all images')
ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

# writing coef into coef.txt
# (описание коэффициентов - добавить - не будь Тимуром)
file = open("coef.txt", "w+")
strr = "3\n3\n"
for i in range(len(mtx[0])):
    for j in range(len(mtx)):
        strr += str(mtx[i, j]) + "\n"
strr += str(len(dist[0])) + "\n"
for i in range(len(dist[0])):
    strr += str(dist[0, i]) + "\n"
file.write(strr)
file.close()

h, w = img.shape[:2]
newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w, h), 1, (w, h))

count = 0
for fname in images:
    dst = cv2.undistort(cv2.imread(fname), mtx, dist, None, newcameramtx)
    x, y, w, h = roi
    dst = dst[y:y + h, x:x + w]
    cv2.imwrite("calib" + str(count) + ".jpg", dst)
    count += 1

while True:
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    ret, frame = video_capture.read()
    # undistortion
    dst = cv2.undistort(frame, mtx, dist, None, newcameramtx)
    # mapx, mapy = cv2.initUndistortRectifyMap(mtx, dist, None, newcameramtx, (w, h), 5)
    # dst = cv2.remap(img, mapx, mapy, cv2.INTER_LINEAR)
    # crop the image
    x, y, w, h = roi
    dst = dst[y:y + h, x:x + w]
    cv2.imshow('frame', dst)

# Close device
video_capture.release()
cv2.destroyAllWindows()

