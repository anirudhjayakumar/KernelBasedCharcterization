from scipy.optimize import curve_fit
import numpy
import sys
from math import log

def normalize_point(value, min_val, max_val):
    return float(value-min_val)/(max_val-min_val)

           def pic_model(x, a, b, c):
#print 'Calling pic_model'
#print 'X=', x, '(a,b,c)=', a, b, c
#print x
           n = x[:,0]
#print n
               p = x[:,1]
#print p
                   retval = []
                            for i in range(len(n)):
                                retval.append(a*float(n[i]) + b*log(p[i]) + c)
#return a*float(n/p) + b*p + c
                                return retval

                                       def pic_eval(x, a, b, c):
#print x, a, b, c
                                       return a*float(x[0]) + b*log(x[1]) + c

                                              kernel_name = sys.argv[1]
                                                      n_particles = int(sys.argv[2])
                                                              procs = int(sys.argv[3])

                                                                      X = []
                                                                      if kernel_name == 'PUSH':
### PUSH ###
                                                                              X = [[419430400,16,69.7725],
                                                                                      [419430400,32,34.70625],
                                                                                      [419430400,64,18.23125],
                                                                                      [419430400,128,10.140625],
                                                                                      [419430400,256,7.03125],
                                                                                      [26214400,128,1.503125],
                                                                                      [104857600,128,4.484375],
                                                                                      [419430400,128,13.0859375],
                                                                                      [838855369,128,22.53125],
                                                                                      [1677721600,128,41.28125],
                                                                                      [209670400,16,34.700625],
                                                                                      [419430400,32,34.703125],
                                                                                      [838913296,64,35.89375],
                                                                                      [1677721600,128,41.578125],
                                                                                      [3355421476,256,46.414]
                                                                                  ]


                                                                      elif kernel_name == 'DEPOSIT':
### DEPOSIT ###
                                                                                  X = [[419430400,16,40.97375],
                                                                                          [419430400,32,20.605625],
                                                                                          [419430400,64,10.615625],
                                                                                          [419430400,128,5.7359375],
                                                                                          [419430400,256,3.9815625],
                                                                                          [26214400,128,0.65234375],
                                                                                          [104857600,128,2.3515625],
                                                                                          [419430400,128,7.625],
                                                                                          [838855369,128,13.40625],
                                                                                          [1677721600,128,23.9609375],
                                                                                          [209670400,16,20.455],
                                                                                          [419430400,32,20.62125],
                                                                                          [838913296,64,21.1765625],
                                                                                          [1677721600,128,24.671875],
                                                                                          [3355421476,256,28.085937]
                                                                                      ]
                                                                      else:
                                                                                      X = []


#print "Original X data:", X

                                                                                              ppp_list = []
                                                                                                      procs_list = []
                                                                                              for item in X:
                                                                                                              item[0] = float(item[0]/item[1]) # change n total particles to per processor count
                                                                                                                          ppp_list.append(item[0])
                                                                                                                          procs_list.append(item[1])

#print 'PPP list:', ppp_list
#print "Data after modification for per processor count:", X
                                                                                                                          ppp_list.append(float(n_particles/procs))
                                                                                                                          procs_list.append(procs)
                                                                                                                          max_procs = max(procs_list)
                                                                                                                                  min_procs = min(procs_list)
                                                                                                                                          max_ppp = max(ppp_list)
                                                                                                                                                  min_ppp = min(ppp_list)

                                                                                                                                                          local_point = [normalize_point(float(n_particles)/procs, min_ppp, max_ppp), normalize_point(procs, min_procs, max_procs)] # input point

                                                                                                                                                                  from scipy.spatial.distance import sqeuclidean
                                                                                                                                                                  from math import exp
                                                                                                                                                                  weights_array = []

#print X
                                                                                                                                                      for item in X:
                                                                                                                                                                          current_item = [normalize_point(item[0], min_ppp, max_ppp), normalize_point(item[1], min_procs, max_procs)]
                                                                                                                                                                                      distance = sqeuclidean(numpy.array(current_item), numpy.array(local_point))
                                                                                                                                                                                              distance = exp(-1.0*(distance*distance))
#distance = 1.0-abs(distance)
                                                                                                                                                                                                      weights_array.append(distance)

                                                                                                                                                                              if sum(weights_array) == 0:
                                                                                                                                                                                                          print "Weights array is", weights_array, "...reverting to unweighted case!"
                                                                                                                                                                                                          weights_array = [1.0]*len(weights_array)

                                                                                                                                                                                                                  dataX = []
                                                                                                                                                                                                                          dataZ = []

                                                                                                                                                                                                      for item in X:
                                                                                                                                                                                                                                  dataX.append( [ float(item[0]), float(item[1]) ] ) # using no. of particles per core and processor count
                                                                                                                                                                                                                                      dataZ.append( float(item[2]) )

                                                                                                                                                                                                                                      dataX = numpy.array(dataX)
                                                                                                                                                                                                                                              dataZ = numpy.array(dataZ)

#print dataX
#print dataZ

                                                                                                                                                                                                                                                      initial_guess = [1.0, 1.0, 1.0]

                                                                                                                                                                                                                                                              optimalparams, covmatrix = curve_fit(pic_model, dataX, dataZ, p0=initial_guess, sigma=weights_array)

                                                                                                                                                                                                                                                                      print '------------------------------------------------'
                                                                                                                                                                                                                                                                      print 'Optimal params:', optimalparams
#print covmatrix
                                                                                                                                                                                                                                                                      print '------------------------------------------------'

                                                                                                                                                                                                                                                                      print 'Model on training set:'
                                                                                                                                                                                                                                                                      for i in range(len(dataX)):
#evaluation_output = pic_eval(dataX[i].tolist(),*optimalparams)
#perc_diff = 100.0*(dataZ[i]-pic_eval(dataX[i].tolist(),*optimalparams))/dataZ[i]
                                                                                                                                                                                                                                                                          print "%d %d" % (dataX[i][0], dataX[i][1]),
                                                                                                                                                                                                                                                                          print "Wt: %.4f Req:  %.4f Pred:  %.4f percent_error: %.4f" %  (weights_array[i], dataZ[i], pic_eval(dataX[i].tolist(),*optimalparams), 100.0*(dataZ[i]-pic_eval(dataX[i].tolist(),*optimalparams))/dataZ[i])

                                                                                                                                                                                                                                                                          '''
                                                                                                                                                                                                                                                                          def pic_model(params, n, p):
                                                                                                                                                                                                                                                                          return params[0]*float(n/p) + params[1]*p + params[2]

                                                                                                                                                                                                                                                                          X = [[419430400, 16, 5510000000],
                                                                                                                                                                                                                                                                          [419430400, 32, 5520000000],
                                                                                                                                                                                                                                                                          [419430400, 64, 5600000000],
                                                                                                                                                                                                                                                                          [419430400, 128,5620000000],

                                                                                                                                                                                                                                                                          [26214400, 128, 352000000],
                                                                                                                                                                                                                                                                          [104857600, 128, 1410000000],
                                                                                                                                                                                                                                                                          [838860800, 128, 11300000000],
                                                                                                                                                                                                                                                                          [1677721600, 128, 22300000000]]

                                                                                                                                                                                                                                                                          dataX = []
                                                                                                                                                                                                                                                                          dataZ = []

                                                                                                                                                                                                                                                                          for item in X:
                                                                                                                                                                                                                                                                          dataX.append([float(item[0]), float(item[1])])
                                                                                                                                                                                                                                                                          dataZ.append(float(item[2]))

                                                                                                                                                                                                                                                                          dataX = numpy.array(dataX)
                                                                                                                                                                                                                                                                          dataZ = numpy.array(dataZ)

                                                                                                                                                                                                                                                                          print dataX
                                                                                                                                                                                                                                                                          print dataZ

                                                                                                                                                                                                                                                                          optimalparams, covmatrix = curve_fit(pic_model, dataX, dataZ)

                                                                                                                                                                                                                                                                          print optimalparams
#p1, success = leastsq(pic_model, p0[:], args=(X))

#print p1, success
                                                                                                                                                                                                                                                                          '''
