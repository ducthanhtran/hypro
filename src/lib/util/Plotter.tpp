#include "Plotter.h"

namespace hypro {

	template<typename Number>
	Plotter<Number>::~Plotter() {
		mOutfile.close();
	}

	template<typename Number>
	void Plotter<Number>::setFilename(const std::string& _filename) {
		mFilename = _filename;
	}

	template<typename Number>
	void Plotter<Number>::updateSettings(gnuplotSettings _settings) {
		mSettings = _settings;
	}

	template<typename Number>
	void Plotter<Number>::plot2d() const {
		mOutfile.open(mFilename + ".plt");

		if(!mObjects.empty()) {
			// set object
			vector_t<Number> min = mObjects[0][0].rawCoordinates();
			vector_t<Number> max = mObjects[0][0].rawCoordinates();

			unsigned objectCount = 1;
			for(const auto& object : mObjects) {
				std::cout << "Create object " << objectCount << std::endl;
				mOutfile << "\n";
				mOutfile << "set object "<< objectCount <<" polygon from \\\n";

				for(unsigned pointIndex = 0; pointIndex < object.size(); ++pointIndex) {
					assert(object[pointIndex].dimension() <= 2); // TODO: Project to 2d
					if(object[pointIndex].dimension() == 0) {
						continue;
					}
					mOutfile << "  " << double(object[pointIndex].at(0));
					
					// update min and max
					min(0) = min(0) < object[pointIndex].at(0) ? min(0) : object[pointIndex].at(0);
					max(0) = max(0) > object[pointIndex].at(0) ? max(0) : object[pointIndex].at(0);
					
					for(unsigned d = 1; d < object[pointIndex].dimension(); ++d) {
						mOutfile << ", " << double(object[pointIndex].at(d));
						// update min and max
						min(d) = min(d) < object[pointIndex].at(d) ? min(d) : object[pointIndex].at(d);
						max(d) = max(d) > object[pointIndex].at(d) ? max(d) : object[pointIndex].at(d);
					}
					mOutfile << " to \\\n";
				}
				//assert(object[object.size()-1].dimension() <= 2); // TODO: Project to 2d	TODO: REINSERT ASSERTION
				//std::cout << double(object[0].at(0)) << std::endl;
				mOutfile << "  " << double(object[0].at(0));
				for(unsigned d = 1; d < object[0].dimension(); ++d) {
					mOutfile << ", " << double(object[0].at(d));
				}
				mOutfile << " fs empty border lc rgb '" << mSettings.color << "'\n";
				++objectCount;
			}
			
			mOutfile << "set term post eps\n";
			mOutfile << "set output \"" << mFilename << ".eps\"";
			mOutfile << "\n";
			mOutfile << "plot ";

			for(unsigned d = 0; d < min.rows(); ++d) {
				mOutfile << "[" << min(d)*1.1 << ":" << max(d)*1.1 << "] ";
			}
			mOutfile << "NaN notitle";
		}
		mOutfile.close();
	}

	template<typename Number>
	void Plotter<Number>::addObject(std::vector<Point<Number>> _points) {
		std::vector<Point<Number>> copyPoints;
		for(const auto& point : _points) {
			//std::cout << __func__ << ": point " << point.rawCoordinates().transpose() << std::endl;
			copyPoints.push_back(Point<Number>(point));
		}
		assert(copyPoints.size() == _points.size());
		grahamScan(copyPoints);
		//for(const auto& point : copyPoints) {
		//	std::cout << __func__ << ": corrected point " << point.rawCoordinates().transpose() << std::endl;
		//}

		mObjects.push_back(copyPoints);
	}

	template<typename Number>
	void Plotter<Number>::init(const std::string& _filename) {
		mOutfile.open(_filename);
	}

	template<typename Number>
	void Plotter<Number>::grahamScan(std::vector<Point<Number>>& _points) {
		if(!_points.empty()) {
			// initialize -> find minimum Point
			Point<Number> min = _points[0];
			std::map<Number, Point<Number>> sortedPoints;
			for(const auto& point : _points) {
				assert(point.dimension() == 2);
				if(point < min) {
					min = point;
				}
			}

			//std::cout << "Minimum: " << min.rawCoordinates().transpose() << std::endl;

			// sort Points according to polar angle -> we have to insert manually (because of double imprecision)
			for(const auto& point : _points) {
				if(point != min) {
					Number angle = point.polarCoordinates(min)[1];
					if(sortedPoints.empty()) {
						sortedPoints.insert(std::make_pair(angle, point));
					}
					else {
						for(auto pos = sortedPoints.begin(); pos != sortedPoints.end(); ) {
							// if equal, take the one with bigger radial component
							Number newAngle = pos->second.polarCoordinates(min)[1];
							if(carl::AlmostEqual2sComplement(angle, newAngle)) {
								// if equal, compare radial coordinate (distance)
								if(pos->second.polarCoordinates(min)[0] < point.polarCoordinates(min)[0]) {
									pos = sortedPoints.erase(pos);
									sortedPoints.insert(std::make_pair(angle, point));
								}
								break;
							}
							// we assume to be sorted, so check all angles, which are smaller or equal for equality - afterwards simply insert
							else if(angle > newAngle) { // not equal and smaller -> continue search (at end, simply insert)
								++pos;
								if(pos == sortedPoints.end()) {
									sortedPoints.insert(std::make_pair(angle, point));
									break;
								}
							}
							else { // larger and not equal until this point -> simply insert
								sortedPoints.insert(std::make_pair(angle, point));
								break;
							}
						}
					}
				}
			}

			//for(const auto& pair : sortedPoints)
			//	std::cout << "sorted: " << pair.first << ", " << pair.second.rawCoordinates().transpose() << std::endl;
			
			// prepare stack -> initialize with 2 points
			std::stack<Point<Number>> stack;
			stack.push(min);
			stack.push(sortedPoints.begin()->second);
			sortedPoints.erase(sortedPoints.begin());
			unsigned i = 0;
			unsigned n = sortedPoints.size();

			// main loop -> check the two topmost elements of the stack and one third, new point 
			while(i<n) {
				Point<Number> p1 = stack.top();
				stack.pop();
				Point<Number> p2 = stack.top();
				stack.pop();
				//std::cout << __func__ << ": " << p2.rawCoordinates().transpose() << " -- " << p1.rawCoordinates().transpose() << " -- " << sortedPoints.begin()->second.rawCoordinates().transpose() << std::endl;
				if(isLeftTurn(p2,p1,sortedPoints.begin()->second)) {
					// reinsert and add new point
					stack.push(p2);
					stack.push(p1);
					stack.push(sortedPoints.begin()->second);
					sortedPoints.erase(sortedPoints.begin());
					++i;
				}
				else {
					// only reinsert second -> equal to removing the topmost object of the stack
					//std::cout << "Drop " << p1.rawCoordinates().transpose() << std::endl;
					stack.push(p2);
					if(stack.size() < 2) {
						// in this case simply insert, as the stack has to contain at least 2 points
						stack.push(sortedPoints.begin()->second);
						sortedPoints.erase(sortedPoints.begin());
						++i;
					}
				}
			}

			// write result
			_points.clear();
			while(!stack.empty()){
				_points.push_back(stack.top());
				stack.pop();
			}

		}
	}

	template<typename Number>
	bool Plotter<Number>::isLeftTurn(const Point<Number>& a, const Point<Number>& b, const Point<Number>& c) {
		assert(a.dimension() == 2);
		assert(b.dimension() == 2);
		assert(c.dimension() == 2);

		Number val = ((b.rawCoordinates()(0) - a.rawCoordinates()(0))*(c.rawCoordinates()(1) - a.rawCoordinates()(1))) - ((c.rawCoordinates()(0)-a.rawCoordinates()(0))*(b.rawCoordinates()(1)-a.rawCoordinates()(1)));
		//Number val = c.polarCoordinates(a,false)[1] - b.polarCoordinates(a,false)[1];

		return (val > 0);
	}
}